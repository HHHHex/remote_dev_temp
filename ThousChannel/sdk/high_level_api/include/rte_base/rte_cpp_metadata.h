/**
 *
 * Agora Real Time Engagement
 * Copyright (c) 2025 Agora IO. All rights reserved.
 *
 */
#pragma once
#include <string>
#include <vector>
#include <memory>
#include "rte_base/c/metadata.h"

#include "rte_base/rte_cpp_error.h"
#include "rte_base/rte_cpp_string.h"

namespace rte {

/**
 * @brief 
 * @technical preview
 */
class MetadataConfig {
  public:
    MetadataConfig() {
      RteMetadataConfigInit(&c_metadata_config, nullptr);
    }

    MetadataConfig(const RteMetadataConfig* other) {
      if(other == nullptr) {
        return;
      }

      RteMetadataConfigInit(&c_metadata_config, nullptr);
      RteMetadataConfigCopy(&c_metadata_config, other, nullptr);
    }

    ~MetadataConfig() {
      RteMetadataConfigDeinit(&c_metadata_config, nullptr);
    }

    /**
     * @brief Set the Record Ts object
     * @technical preview
     * @param record_ts 
     * @param err 
     */
    void SetRecordTs(bool record_ts, Error *err = nullptr) {
      RteMetadataConfigSetRecordTs(&c_metadata_config, record_ts,
                                   err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Record Ts object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetRecordTs(Error *err = nullptr) {
      bool record_ts;
      RteMetadataConfigGetRecordTs(&c_metadata_config, &record_ts,
                                   err != nullptr ? err->get_underlying_impl() : nullptr);
      return record_ts;
    }

    /**
     * @brief Set the Record Owner object
     * @technical preview
     * @param record_owner 
     * @param err 
     */
    void SetRecordOwner(bool record_owner, Error *err = nullptr) {
      RteMetadataConfigSetRecordOwner(&c_metadata_config, record_owner,
                                      err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Record Owner object
     * @technical preview
     * @param err 
     * @return bool 
     */
    bool GetRecordOwner(Error *err = nullptr) {   
      bool record_owner;
      RteMetadataConfigGetRecordOwner(&c_metadata_config, &record_owner,
                                      err != nullptr ? err->get_underlying_impl() : nullptr);
      return record_owner;
    }
    
    void SetJsonParameter(const std::string& json_parameter, Error *err = nullptr) {
      String str(json_parameter.c_str());
      RteMetadataConfigSetJsonParameter(&c_metadata_config, str.get_underlying_impl(),
                                        err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    std::string GetJsonParameter(Error *err = nullptr) {  
      String str;
      RteMetadataConfigGetJsonParameter(&c_metadata_config, str.get_underlying_impl(),
                                        err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    ::RteMetadataConfig* get_underlying_impl() {
      return &c_metadata_config;
    }
    
  private:
    ::RteMetadataConfig c_metadata_config;
};

/**
 * @brief 
 * @technical preview
 */
class MetadataItem {
  public:
    MetadataItem() {
      RteMetadataItemInit(&c_metadata_item, nullptr);
    }

    ~MetadataItem() {
      RteMetadataItemDeinit(&c_metadata_item, nullptr);
    }

    MetadataItem(const RteMetadataItem* other) {
      if(other == nullptr) {
        return;
      }

      RteMetadataItemInit(&c_metadata_item, nullptr);

      if(other->key != nullptr) {
        RteMetadataItemSetKey(&c_metadata_item, other->key, nullptr);
      }

      if(other->value != nullptr) {
        RteMetadataItemSetValue(&c_metadata_item, other->value, nullptr);
      }

      if(other->author != nullptr) {
        RteMetadataItemSetAuthor(&c_metadata_item, other->author, nullptr);
      }

      RteMetadataItemSetRevision(&c_metadata_item, other->revision, nullptr);
      RteMetadataItemSetUpdateTimestamp(&c_metadata_item, other->update_timestamp, nullptr);
    }

    MetadataItem(MetadataItem& other) {
      RteMetadataItemInit(&c_metadata_item, nullptr);

      if(other.c_metadata_item.key != nullptr) {
        RteMetadataItemSetKey(&c_metadata_item, other.c_metadata_item.key, nullptr);
      }

      if(other.c_metadata_item.value != nullptr) {
        RteMetadataItemSetValue(&c_metadata_item, other.c_metadata_item.value, nullptr);
      }
      
      if(other.c_metadata_item.author != nullptr) {
        RteMetadataItemSetAuthor(&c_metadata_item, other.c_metadata_item.author, nullptr);
      }

      RteMetadataItemSetRevision(&c_metadata_item, other.c_metadata_item.revision, nullptr);
      RteMetadataItemSetUpdateTimestamp(&c_metadata_item, other.c_metadata_item.update_timestamp, nullptr);
    }

    MetadataItem(MetadataItem&& other) {
      RteMetadataItemInit(&c_metadata_item, nullptr);
      c_metadata_item = other.c_metadata_item;
      other.c_metadata_item.key = nullptr;
      other.c_metadata_item.value = nullptr;
      other.c_metadata_item.author = nullptr;
    }

    /**
     * @brief Set the Key object
     * @technical preview
     * @param key 
     * @param err 
     */
    void SetKey(const std::string& key, Error *err = nullptr) {
      String str(key.c_str());
      RteMetadataItemSetKey(&c_metadata_item, str.get_underlying_impl(),
                            err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Key object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetKey(Error *err = nullptr) {
      String str;
      RteMetadataItemGetKey(&c_metadata_item, str.get_underlying_impl(),
                            err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Set the Value object
     * @technical preview
     * @param value 
     * @param err 
     */
    void SetValue(const std::string& value, Error *err = nullptr) {
      String str(value.c_str());
      RteMetadataItemSetValue(&c_metadata_item, str.get_underlying_impl(),
                              err != nullptr ? err->get_underlying_impl() : nullptr);
    } 

    /**
     * @brief Get the Value object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetValue(Error *err = nullptr) {
      String str;
      RteMetadataItemGetValue(&c_metadata_item, str.get_underlying_impl(),
                              err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Set the Author object
     * @technical preview
     * @param author 
     * @param err 
     */
    void SetAuthor(const std::string& author, Error *err = nullptr) {
      String str(author.c_str());
      RteMetadataItemSetAuthor(&c_metadata_item, str.get_underlying_impl(),
                              err != nullptr ? err->get_underlying_impl() : nullptr);
    } 

    /**
     * @brief Get the Author object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetAuthor(Error *err = nullptr) {
      String str;
      RteMetadataItemGetAuthor(&c_metadata_item, str.get_underlying_impl(),
                              err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(str.CStr());
    }

    /**
     * @brief Set the Revision object
     * @technical preview
     * @param revision 
     * @param err 
     */
    void SetRevision(int64_t revision, Error *err = nullptr) {
      RteMetadataItemSetRevision(&c_metadata_item, revision,
                                err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Revision object
     * @technical preview
     * @param err 
     * @return int64_t 
     */
    int64_t GetRevision(Error *err = nullptr) { 
      int64_t revision;
      RteMetadataItemGetRevision(&c_metadata_item, &revision,
                                err != nullptr ? err->get_underlying_impl() : nullptr);
      return revision;
    } 

    /**
     * @brief Set the Update Timestamp object
     * @technical preview
     * @param update_timestamp 
     * @param err 
     */
    void SetUpdateTimestamp(int64_t update_timestamp, Error *err = nullptr) {
      RteMetadataItemSetUpdateTimestamp(&c_metadata_item, update_timestamp,
                                        err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Update Timestamp object
     * @technical preview
     * @param err 
     * @return int64_t 
     */
    int64_t GetUpdateTimestamp(Error *err = nullptr) {    
      int64_t update_timestamp;
      RteMetadataItemGetUpdateTimestamp(&c_metadata_item, &update_timestamp,
                                        err != nullptr ? err->get_underlying_impl() : nullptr);
      return update_timestamp;
    }


    ::RteMetadataItem* get_underlying_impl() {
      return &c_metadata_item;
    }
    
  private:
    ::RteMetadataItem c_metadata_item;

};

/**
 * @brief 
 * @technical preview
 */
class Metadata {
  public:
    Metadata() {
      RteMetadataInit(&c_metadata, nullptr);
    }

    ~Metadata() {
      RteMetadataDeinit(&c_metadata, nullptr);
    }

    Metadata(const Metadata& other) {
      RteMetadataInit(&c_metadata, nullptr);

      RteMetadataSetRevision(&c_metadata, other.c_metadata.revision, nullptr);

      for(size_t i = 0; i < other.c_metadata.items_cnt; i++) {
        RteMetadataAddItem(&c_metadata, &other.c_metadata.items[i], nullptr);
      }
    }

    Metadata(const RteMetadata* other) {
      RteMetadataInit(&c_metadata, nullptr);

      if(other == nullptr) {
        return;
      }

      RteMetadataSetRevision(&c_metadata, other->revision, nullptr);

      for(size_t i = 0; i < other->items_cnt; i++) {
        RteMetadataAddItem(&c_metadata, &other->items[i], nullptr);
      }
    }

    Metadata& operator=(const Metadata& other) {
      RteMetadataClear(&c_metadata, nullptr);

      RteMetadataSetRevision(&c_metadata, other.c_metadata.revision, nullptr);
      for(size_t i = 0; i < other.c_metadata.items_cnt; i++) {
        RteMetadataAddItem(&c_metadata, &other.c_metadata.items[i], nullptr);
      }

      return *this;
    }

    /**
     * @brief Set the Revision object
     * @technical preview
     * @param revision 
     * @param err 
     */
    void SetRevision(int64_t revision, Error *err = nullptr) {
      RteMetadataSetRevision(&c_metadata, revision,
                             err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Revision object
     * @technical preview
     * @param err 
     * @return int64_t 
     */
    int64_t GetRevision(Error *err = nullptr) {
      int64_t revision;
      RteMetadataGetRevision(&c_metadata, &revision,
                             err != nullptr ? err->get_underlying_impl() : nullptr);
      return revision;
    }

    void Clear(Error *err = nullptr) {
      RteMetadataClear(&c_metadata,
                       err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Add the Item object
     * @technical preview
     * @param item 
     * @param err 
     */
    void AddItem(MetadataItem* item, Error *err = nullptr) {
      RteMetadataAddItem(&c_metadata, item != nullptr ? item->get_underlying_impl() : nullptr,
                         err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Items object
     * @technical preview
     * @param items 
     * @param err 
     */
    void GetItems(std::vector<MetadataItem>& items, Error *err = nullptr){
      size_t items_cnt = RteMetadataGetItems(&c_metadata, nullptr, 0, nullptr);
      std::shared_ptr<RteMetadataItem> metadatas(new RteMetadataItem[items_cnt], [](RteMetadataItem * ptr){ delete [] ptr; ptr = nullptr;});
      for (int index = 0; index < items_cnt; index++) {
          RteMetadataItemInit(&metadatas.get()[index], err != nullptr ? err->get_underlying_impl() : nullptr);
      }
      RteMetadataGetItems(&c_metadata, metadatas.get(), items_cnt, err != nullptr ? err->get_underlying_impl() : nullptr);

      for (size_t i = 0; i < items_cnt; i++) {
        items.push_back(MetadataItem(&metadatas.get()[i]));
      }
    }
    
  
    void CopyTo(RteMetadata* other) {
      if(other == nullptr) {
        return;
      }
      
      RteMetadataClear(other, nullptr);
      for(size_t i = 0; i < c_metadata.items_cnt; i++) {
        RteMetadataAddItem(other, &c_metadata.items[i], nullptr);
      }

      other->revision = c_metadata.revision;

    }

    ::RteMetadata* get_underlying_impl() {
      return &c_metadata;
    }
    
  private:
    ::RteMetadata c_metadata;
};  


class MetadataInfo {
  public:
    MetadataInfo(){RteMetadataInfoInit(&c_metadata_info, nullptr);}
    ~MetadataInfo(){RteMetadataInfoDeinit(&c_metadata_info, nullptr);}

    MetadataInfo(const MetadataInfo& other){
      RteMetadataInfoInit(&c_metadata_info, nullptr);
      RteMetadataInfoCopy(&c_metadata_info, &other.c_metadata_info, nullptr);
    }

    explicit MetadataInfo(RteMetadataInfo* other){
      RteMetadataInfoInit(&c_metadata_info, nullptr);
      if(other != nullptr){
        RteMetadataInfoCopy(&c_metadata_info, other, nullptr);  
      }
    }

    MetadataInfo& operator=(const MetadataInfo& other){
      RteMetadataInfoDeinit(&c_metadata_info, nullptr);
      RteMetadataInfoCopy(&c_metadata_info, &other.c_metadata_info, nullptr);
      return *this;
    }



    void SetTarget(const std::string& target, Error *err = nullptr) {
      String str(target.c_str());
      RteMetadataInfoSetTarget(&c_metadata_info, str.get_underlying_impl(), 
                              err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Target object
     * @technical preview
     * @param err 
     * @return std::string 
     */
    std::string GetTarget(Error *err = nullptr) {
      String rte_string;
      RteMetadataInfoGetTarget(&c_metadata_info, rte_string.get_underlying_impl(), err != nullptr ? err->get_underlying_impl() : nullptr);
      return std::string(rte_string.CStr());
    }


    void SetTimestamp(uint64_t timestamp, Error *err = nullptr) {
      RteMetadataInfoSetTimestamp(&c_metadata_info, timestamp, err != nullptr ? err->get_underlying_impl() : nullptr);
    }

    /**
     * @brief Get the Timestamp object
     * @technical preview
     * @param err 
     * @return uint64_t 
     */
    uint64_t GetTimestamp(Error *err = nullptr) {
      uint64_t timestamp = 0;
      RteMetadataInfoGetTimestamp(&c_metadata_info, &timestamp, err != nullptr ? err->get_underlying_impl() : nullptr);
      return timestamp;
    }


    ::RteMetadataInfo* get_underlying_impl(){
      return &c_metadata_info;
    }

  private:
    ::RteMetadataInfo c_metadata_info;
};

}
