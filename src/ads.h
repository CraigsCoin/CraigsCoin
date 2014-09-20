#pragma once

#include "util.h"
#include <string>

static const int64_t AD_FEE_AMOUNT = 5 * COIN;
static const char AD_FEE_ADDRESS[] = "CHjJ6sJ7PpnPhc4XYHmgNY2YHLZeR4QueT";
static const char AD_FEE_ADDRESS_TESTNET[] = "mmrBgTm1ycomXHdSooj3gf6zkf1FGDGhqY";


struct AdEntry
{
  struct Person
  {
    std::string name;
    std::string contacts;
  };
  struct Location
  {
    std::string country;
    std::string state;
    std::string city;
    std::string street_address;
    std::string postal_code;
  };
  struct Category
  {
    std::string level_1;
    std::string level_2;
  };
  struct Ad
  {
    std::string title;
    std::string description;
    typedef std::vector<std::string> Tags;
    Tags tags;
    std::string price;
    typedef std::vector<std::string> Images;
    Images images;
    typedef std::vector<std::string> Videos;
    Videos videos;
  };
  Person person;
  Location location;
  Category category;
  Ad ad;

  void parse(const std::string& json);
  std::string format() const;
};