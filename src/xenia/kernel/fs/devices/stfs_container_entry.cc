/**
 ******************************************************************************
 * Xenia : Xbox 360 Emulator Research Project                                 *
 ******************************************************************************
 * Copyright 2014 Ben Vanik. All rights reserved.                             *
 * Released under the BSD license - see LICENSE in the root for more details. *
 ******************************************************************************
 */

#include <xenia/kernel/fs/devices/stfs_container_entry.h>

#include <xenia/kernel/fs/stfs.h>
#include <xenia/kernel/fs/devices/stfs_container_file.h>

namespace xe {
namespace kernel {
namespace fs {

STFSContainerEntry::STFSContainerEntry(Type type, Device* device,
                                       const char* path,
                                       poly::MappedMemory* mmap,
                                       STFSEntry* stfs_entry)
    : Entry(type, device, path),
      mmap_(mmap),
      stfs_entry_(stfs_entry),
      stfs_entry_iterator_(stfs_entry->children.end()) {}

STFSContainerEntry::~STFSContainerEntry() = default;

X_STATUS STFSContainerEntry::QueryInfo(XFileInfo* out_info) {
  assert_not_null(out_info);
  out_info->creation_time = stfs_entry_->update_timestamp;
  out_info->last_access_time = stfs_entry_->access_timestamp;
  out_info->last_write_time = stfs_entry_->update_timestamp;
  out_info->change_time = stfs_entry_->update_timestamp;
  out_info->allocation_size = 4096;
  out_info->file_length = stfs_entry_->size;
  out_info->attributes = stfs_entry_->attributes;
  return X_STATUS_SUCCESS;
}

X_STATUS STFSContainerEntry::QueryDirectory(XDirectoryInfo* out_info,
                                            size_t length,
                                            const char* file_name,
                                            bool restart) {
  assert_not_null(out_info);

  if (restart && stfs_entry_iterator_ != stfs_entry_->children.end()) {
    stfs_entry_iterator_ = stfs_entry_->children.end();
  }

  if (stfs_entry_iterator_ == stfs_entry_->children.end()) {
    stfs_entry_iterator_ = stfs_entry_->children.begin();
    if (stfs_entry_iterator_ == stfs_entry_->children.end()) {
      return X_STATUS_UNSUCCESSFUL;
    }
  } else {
    ++stfs_entry_iterator_;
    if (stfs_entry_iterator_ == stfs_entry_->children.end()) {
      return X_STATUS_UNSUCCESSFUL;
    }
  }

  auto end = (uint8_t*)out_info + length;

  auto entry = stfs_entry_iterator_->get();
  auto entry_name = entry->name;

  if (((uint8_t*)&out_info->file_name[0]) + entry_name.size() > end) {
    stfs_entry_iterator_ = stfs_entry_->children.end();
    return X_STATUS_UNSUCCESSFUL;
  }

  out_info->file_index = 0xCDCDCDCD;
  out_info->creation_time = entry->update_timestamp;
  out_info->last_access_time = entry->access_timestamp;
  out_info->last_write_time = entry->update_timestamp;
  out_info->change_time = entry->update_timestamp;
  out_info->end_of_file = entry->size;
  out_info->allocation_size = 4096;
  out_info->attributes = entry->attributes;
  out_info->file_name_length = static_cast<uint32_t>(entry_name.size());
  memcpy(out_info->file_name, entry_name.c_str(), entry_name.size());

  return X_STATUS_SUCCESS;
}

X_STATUS STFSContainerEntry::Open(KernelState* kernel_state, Mode mode,
                                  bool async, XFile** out_file) {
  *out_file = new STFSContainerFile(kernel_state, mode, this);
  return X_STATUS_SUCCESS;
}

}  // namespace fs
}  // namespace kernel
}  // namespace xe
