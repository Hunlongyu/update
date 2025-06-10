#include "request.h"
#include "hv/HttpClient.h"
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

int ClientRequest::downloadAsset(const Asset &asset, std::function<void(size_t downloaded, size_t total)> progress_cb)
{
    using namespace hv;
    cancel_flag = false;
    std::string url = asset.browser_download_url;
    if (url.empty())
    {
        return -1;
    }
    std::string filename = asset.name;
    if (filename.empty())
    {
        return -1;
    }

    std::string download_dir = "update/";
    std::string filepath_download = download_dir + filename + ".download";
    std::string filepath_final = download_dir + filename;
    _mkdir(download_dir.c_str());

    HttpClient cli;
    HttpRequest req;
    HttpResponse resp;

    req.method = HTTP_HEAD;
    req.url = url;
    req.timeout = 3600;
    int ret = cli.send(&req, &resp);
    if (ret != 0)
    {
        return ret;
    }

    size_t content_length = resp.content_length;
    if (content_length == 0)
    {
        content_length = asset.size;
    }

    HFile file;
    ret = file.open(filepath_download.c_str(), "wb");
    if (ret != 0)
    {
        return ret;
    }

    req.method = HTTP_GET;
    size_t downloaded = 0;
    req.http_cb = [this, &file, &downloaded, &content_length, &progress_cb](HttpMessage *resp, http_parser_state state, const char *data, size_t size) {
        if (state == HP_BODY)
        {
            if (cancel_flag)
            {
                return -1;
            }
            if (data && size)
            {
                file.write(data, size);
                downloaded += size;
                if (progress_cb)
                {
                    progress_cb(downloaded, content_length);
                }
            }
        }
    };

    ret = cli.send(&req, &resp);
    file.close();

    if (ret == 0)
    {
        file.rename(filepath_final.c_str());
    }
    else
    {
        file.remove();
    }
    return ret;
}

void ClientRequest::cancelDownload()
{
    cancel_flag = true;
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