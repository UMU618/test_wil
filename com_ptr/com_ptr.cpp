#include <tchar.h>

#include <format>
#include <iostream>
#include <map>

#include <dxgi1_5.h>

#include <wil/com.h>

#pragma comment(lib, "dxgi.lib")

#ifdef _UNICODE
#define tcout std::wcout
#else
#define tcout std::cout
#endif

using tstring = std::basic_string<TCHAR>;

constexpr std::uint32_t kVendorIdNvidia = 4318;  // 0x10de

struct AdapterInfo {
  LUID luid;
  tstring name;
  std::uint32_t vendor_id;  // PCI ID of vendor
  std::uint32_t device_id;
  std::uint32_t sub_sys_id;
  std::uint32_t revision;

  bool isNvidia() const noexcept { return kVendorIdNvidia == vendor_id; }
};
bool operator<(const LUID& lhs, const LUID& rhs) {
  return LARGE_INTEGER{.LowPart = lhs.LowPart, .HighPart = lhs.HighPart}
             .QuadPart <
         LARGE_INTEGER{.LowPart = rhs.LowPart, .HighPart = rhs.HighPart}
             .QuadPart;
}
using AdaptersById = std::map<LUID, AdapterInfo>;

AdaptersById GetAdaptersById(bool exclude_software_adapter = true) noexcept {
  AdaptersById adapters_by_id;
  wil::com_ptr_nothrow<IDXGIFactory1> dxgi_factory1;
  if (FAILED_LOG(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory1)))) {
    return adapters_by_id;
  }

  UINT i = 0;
  for (wil ::com_ptr_nothrow<IDXGIAdapter1> adapter;
       DXGI_ERROR_NOT_FOUND != dxgi_factory1->EnumAdapters1(i, adapter.put());
       ++i) {
    DXGI_ADAPTER_DESC1 desc;
    if (FAILED_LOG(adapter->GetDesc1(&desc))) {
      continue;
    }
    if (exclude_software_adapter &&
        (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
      continue;
    }

    adapters_by_id[desc.AdapterLuid] = AdapterInfo{.luid = desc.AdapterLuid,
                                                   .name = desc.Description,
                                                   .vendor_id = desc.VendorId,
                                                   .device_id = desc.DeviceId,
                                                   .sub_sys_id = desc.SubSysId,
                                                   .revision = desc.Revision};
  }
  return adapters_by_id;
}

int main() {
  AdaptersById adapters_by_id = GetAdaptersById(false);
  for (const auto& adapter : adapters_by_id) {
    std::cout << std::format("{:08x}-{:08X}\n", adapter.second.luid.HighPart,
                             adapter.second.luid.LowPart);
    tcout << std::format(_T("  Name    : {}\n"), adapter.second.name);
    std::cout << std::format("  IsNvidia: {}\n", adapter.second.isNvidia());
    std::cout << std::format("  VendorId: {}\n", adapter.second.vendor_id);
    std::cout << std::format("  DeviceId: {}\n", adapter.second.device_id);
    std::cout << std::format("  SubSysId: {}\n", adapter.second.sub_sys_id);
    std::cout << std::format("  Revision: {}\n", adapter.second.revision);
  }
}
