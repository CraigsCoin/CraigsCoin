#include <ads.h>

#include <sstream>

#include <boost/foreach.hpp>

#include "json/json_spirit_reader_template.h"
#include "json/json_spirit_writer_template.h"
#include "json/json_spirit_utils.h"

using namespace json_spirit;


void AdEntry::parse(const std::string& json)
{
  mValue val;
  if (!read_string(json, val))
  {
    throw std::logic_error("read_string");
  }

  mObject result(val.get_obj());
  {
    mObject person(result["person"].get_obj());
    this->person.name = person["name"].get_str();
    this->person.contacts = person["contacts"].get_str();
  }
  {
    mObject location(result["location"].get_obj());
    this->location.country = location["country"].get_str();
    this->location.state = location["state"].get_str();
    this->location.city = location["city"].get_str();
    this->location.street_address = location["street_address"].get_str();
    this->location.postal_code = location["postal_code"].get_str();
  }
  {
    mObject category(result["category"].get_obj());
    this->category.level_1 = category["level_1"].get_str();
    this->category.level_2 = category["level_2"].get_str();
  }
  {
    mObject ad(result["ad"].get_obj());
    this->ad.title = ad["title"].get_str();
    this->ad.description = ad["description"].get_str();
    {
      mArray tags(ad["tags"].get_array());
      BOOST_FOREACH(const mValue& item, tags)
      {
        this->ad.tags.push_back(item.get_str());
      }
    }
    this->ad.price = ad["price"].get_str();
    {
      mArray images(ad["images"].get_array());
      BOOST_FOREACH(const mValue& item, images)
      {
        this->ad.images.push_back(item.get_str());
      }
    }
    {
      mArray videos(ad["videos"].get_array());
      BOOST_FOREACH(const mValue& item, videos)
      {
        this->ad.videos.push_back(item.get_str());
      }
    }
  }
}

std::string AdEntry::format() const
{
  Object result;
  {
    Object person;
    person.push_back(Pair("name", this->person.name));
    person.push_back(Pair("contacts", this->person.contacts));
    result.push_back(Pair("person", Value(person)));
  }
  {
    Object location;
    location.push_back(Pair("country", this->location.country));
    location.push_back(Pair("state", this->location.state));
    location.push_back(Pair("city", this->location.city));
    location.push_back(Pair("street_address", this->location.street_address));
    location.push_back(Pair("postal_code", this->location.postal_code));
    result.push_back(Pair("location", Value(location)));
  }
  {
    Object category;
    category.push_back(Pair("level_1", this->category.level_1));
    category.push_back(Pair("level_2", this->category.level_2));
    result.push_back(Pair("category", Value(category)));
  }
  {
    Object ad;
    ad.push_back(Pair("title", this->ad.title));
    ad.push_back(Pair("description", this->ad.description));
    {
      Array tags;
      BOOST_FOREACH(const std::string& item, this->ad.tags)
      {
        tags.push_back(item);
      }
      ad.push_back(Pair("tags", Value(tags)));
    }
    ad.push_back(Pair("price", this->ad.price));
    {
      Array images;
      BOOST_FOREACH(const std::string& item, this->ad.images)
      {
        images.push_back(item);
      }
      ad.push_back(Pair("images", Value(images)));
    }
    {
      Array videos;
      BOOST_FOREACH(const std::string& item, this->ad.videos)
      {
        videos.push_back(item);
      }
      ad.push_back(Pair("videos", Value(videos)));
    }
    result.push_back(Pair("ad", Value(ad)));
  }
  {
    std::stringstream ss;
    write_stream(Value(result), ss, false);
    return ss.str();
  }
}


