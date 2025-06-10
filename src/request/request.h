#pragma once
#include <mutex>

#include "hv/axios.h"

using json = nlohmann::json;

struct Asset
{
    size_t size;
    std::string name;
    std::string updated_at;
    std::string browser_download_url;
};

struct Release
{
    std::string tag_name;
    std::vector<Asset> assets;
    std::string body;
};

class ClientRequest
{
  public:
    ClientRequest();
    ~ClientRequest();

    // 获取最新的一个 release
    static Release getLatestRelease(const std::string &url);

    // 下载资源 并显示进度
    int downloadAsset(const Asset &asset, std::function<void(size_t downloaded, size_t total)> progress_cb);

    // 取消下载
    void cancelDownload();

  private:
    std::string releases;                 // 缓存 release
    std::atomic<bool> cancel_flag{false}; // 下载取消标志
    std::mutex download_mutex;            // 多线程 下载锁

    // url 转换
    static std::string urlConvert(const std::string &url);

    // 解析 release
    static std::vector<Release> parseReleases(const std::string &releases);
};