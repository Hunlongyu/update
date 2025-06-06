#include "request.h"
#include "hv/hlog.h"
#include "nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

ClientRequest::ClientRequest()
{
    hlog_disable();
}

ClientRequest::~ClientRequest()
{
}

Release ClientRequest::getLatestRelease(const std::string &url)
{
    const auto api = urlConvert(url);
    json json;
    json["method"] = "GET";
    json["url"] = api + "?per_page=1";
    json["headers"] = {{"Accept", "application/vnd.github.v3+json"},
                       {"User-Agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/605.1.15 (KHTML, like Gecko) Chrome/136.0.7103.114 Safari/605.1.15"},
                       {"Accept-Charset", "utf-8"},
                       {"Content-Type", "application/json; charset=utf-8"}};
    auto resp = axios::axios(json);
    if (resp == nullptr)
    {
        return Release();
    }
    if (resp->status_code != 200)
    {
        return Release();
    }
    if (resp->body.empty())
    {
        return Release();
    }

    const auto releases = parseReleases(resp->body);
    if (releases.empty())
    {
        return Release();
    }

    return releases[0];
}

std::vector<Release> ClientRequest::getReleases(const std::string &url)
{
    return std::vector<Release>();
}

std::vector<Release> ClientRequest::parseReleases(const std::string &releases)
{
    std::vector<Release> result;
    json arr;
    try
    {
        arr = json::parse(releases);
    }
    catch (json::exception &e)
    {
        return result;
    }

    if (!arr.is_array())
    {
        return result;
    }

    for (const auto &rel : arr)
    {
        Release r;
        r.tag_name = rel.value("tag_name", "");
        r.body = rel.value("body", "");

        const auto &assets_arr = rel.value("assets", json::array());
        for (const auto &a : assets_arr)
        {
            Asset asset;
            asset.name = a.value("name", "");
            asset.size = a.value("size", 0);
            asset.updated_at = a.value("updated_at", "");
            asset.browser_download_url = a.value("browser_download_url", "");
            r.assets.push_back(asset);
        }
        result.push_back(r);
    }
    return result;
}

void ClientRequest::downloadAsset(Asset asset, std::function<void(size_t downloaded, size_t total)> progress_cb)
{
    return;
}

void ClientRequest::cancelDownload()
{
    return;
}

std::string ClientRequest::urlConvert(const std::string &url)
{
    // 1. 查找 "github.com/"
    std::string prefix = "github.com/";
    auto pos = url.find(prefix);
    if (pos == std::string::npos)
    {
        return "";
    }
    // 2. 提取 owner/repo
    std::string repo_path = url.substr(pos + prefix.length());
    // 3. 去除结尾斜杠
    if (!repo_path.empty() && repo_path.back() == '/')
    {
        repo_path.pop_back();
    }
    // 4. 拼接API地址
    return "http://api.github.com/repos/" + repo_path + "/releases";
}