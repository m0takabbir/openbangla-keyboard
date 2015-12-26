/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015 Muhammad Mominul Huque <nahidbinbaten1995@gmail.com>
 *
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

 /*
  *  Actually this parser is originated from the parser written by
  *  Rifat Nabi for iAvro under MPL 1.1. It was written in Objective C.
  *  I(Muhammad Mominul Huque) have re-written the parser in C++ for using with
  *  OpenBangla Keyboard.
  *  For showing respect to the Avro project and Rifat Nabi, I am
  *  releasing this code under MPL 2.0.
  *  So this is code now dual licensed under the MPL 2 and the GNU GPL 3.
  *  If you have any questions about this matter, please send e-mail to
  *  me at described above.
  *  http://www.gnu.org/licenses/license-list.en.html#MPL-2.0
  */

#include "regexparser.h"

using namespace nlohmann;

RegexParser::RegexParser() {
  fin.open(PKGDATADIR "/data/regex.json", std::ifstream::in);

  grammar << fin;

  patterns = grammar["patterns"];
  std::string _find = patterns[0]["find"];
  maxPatternLength = _find.length();
  vowel = grammar["vowel"];
  cons = grammar["consonant"];
  ign = grammar["ignore"];
}

RegexParser::~RegexParser() {
  // Close the file handler
  fin.close();
}

std::string RegexParser::parse(std::string input) {
  // Check
  if(input.length() == 0) return input;

  std::string fixed = cleanString(input);
  std::string output;

  int len = fixed.length();
  for(int cur = 0; cur < len; ++cur) {
    int start = cur, end;
    bool matched = false;

    int chunkLen;
    for(int chunkLen = maxPatternLength; chunkLen > 0; --chunkLen) {
      end = start + chunkLen;
      if(end <= len) {
        std::string chunk = fixed.substr(start, chunkLen);

        // Binary Search
        int left = 0, right = patterns.size() - 1, mid;
        while(right >= left) {
          mid = (right + left) / 2;
          json pattern = patterns.at(mid);
          std::string find = pattern["find"];
          if(find == chunk) {
            json rules = pattern["rules"];
            if(!(rules.empty())) {
              for(auto& rule : rules) {
                bool replace = true;
                int chk = 0;
                json matches = rule.at("matches");
                for(auto& match : matches) {
                  std::string value = match["value"];
                  std::string type = match["type"];
                  std::string scope = match["scope"];
                  bool isNegative = match["negative"];

                  if(type == "suffix") {
                    chk = end;
                  }
                  // Prefix
                  else {
                    chk = start - 1;
                  }

                  // Beginning
                  if(scope == "punctuation") {
                    if(
                      ! (
                        (chk < 0 && (type == "prefix")) ||
                        (chk >= len && (type == "suffix")) ||
                        isPunctuation(fixed.at(chk))
                        ) ^ isNegative
                      ) {
                            replace = false;
                            break;
                    }
                  }
                  // Vowel
                  else if(scope == "vowel") {
                    if(
                       !(
                        (
                        (chk >= 0 && (type == "prefix")) ||
                        (chk < len && (type == "suffix"))
                        ) &&
                        isVowel(fixed.at(chk))
                        ) ^ isNegative
                      ) {
                            replace = false;
                            break;
                    }
                  }
                  // Consonant
                  else if(scope == "consonant") {
                    if(
                       !(
                        (
                        (chk >= 0 && (type == "prefix")) ||
                        (chk < len && (type == "suffix"))
                        ) &&
                        isConsonant(fixed.at(chk))
                        ) ^ isNegative
                      ) {
                            replace = false;
                            break;
                    }
                  }
                  // Exact
                  else if(scope == "exact") {
                    int s, e;
                    if(type == "suffix") {
                      s = end;
                      e = end + value.length();
                    }
                    // Prefix
                    else {
                      s = start - value.length();
                      e = start;
                    }
                    if(!(isExact(value, fixed, s, e, isNegative))) {
                      replace = false;
                      break;
                    }
                  }
                }

                if(replace) {
                  std::string rl = rule["replace"];
                  output += rl;
                  output += "(্[যবম])?(্?)([ঃঁ]?)";
                  cur = end - 1;
                  matched = true;
                  break;
                }
              }
            }

            if(matched == true) break;

            // Default
            std::string rl = pattern["replace"];
            output += rl;
            output += "(্[যবম])?(্?)([ঃঁ]?)";
            cur = end - 1;
            matched = true;
            break;
          }
          else if (find.length() > chunk.length() ||
                  (find.length() == chunk.length() && find.compare(chunk) < 0)) {
                  left = mid + 1;
          } else {
            right = mid - 1;
          }
        }
        if(matched == true) break;
      }
    }

    if(!matched) {
      output += fixed.at(cur);
    }
  }

  return output;
}

/* Convert to their returning type. Warning: We only convert one character! */
char RegexParser::to_char(std::string a) {const char* b = a.c_str(); char r = *b; return r;}
std::string RegexParser::to_str(char a) {std::string r; r = a; return r;}

char RegexParser::smallCap(char letter) {
    std::string res = to_str(letter);
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return to_char(res);
}

std::string RegexParser::cleanString(std::string input) {
  std::string fixed;
  for(const auto& c : input) {
    if(!isIgnore(c)) {
      fixed += smallCap(c);
    }
  }
  return fixed;
}

bool RegexParser::isVowel(char c) {
  return vowel.find(smallCap(c)) != std::string::npos;
}

bool RegexParser::isConsonant(char c) {
  return cons.find(smallCap(c)) != std::string::npos;
}

bool RegexParser::isPunctuation(char c) {
  return (!(isVowel(c) || isConsonant(c)));
}

bool RegexParser::isExact(std::string needle, std::string heystack, int start, int end, bool strnot) {
  int len = end - start;
  return ((start >= 0 && end < heystack.length() && (heystack.substr(start, len)  == needle)) ^ strnot);
}

bool RegexParser::isIgnore(char c) {
  return ign.find(smallCap(c)) != std::string::npos;
}