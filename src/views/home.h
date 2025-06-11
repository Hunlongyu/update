#pragma once
#define _WINSOCKAPI_
#include <SimpleWindow.h>

#include "src/request/request.h"
#include "src/utils/utils.h"

class Home : public sw::Window
{
  public:
    Home(const Args &args);
    ~Home() override;

  protected:
    virtual LRESULT WndProc(const sw::ProcMsg &refMsg) override;

  private:
    sw::Grid *m_grid{nullptr};                // 网格布局
    sw::StackPanel *m_stack_panel{nullptr};   // 版本布局
    sw::Label *m_current_version{nullptr};    // 当前版本
    sw::Label *m_latest_version{nullptr};     // 最新版本
    sw::Label *m_logs_tips{nullptr};          // 更新日志提示
    sw::ListBox *m_update_logs{nullptr};      // 更新日志
    sw::ProgressBar *m_progress_bar{nullptr}; // 进度条
    sw::Grid *m_btns_grid{nullptr};           // 按钮组
    sw::Button *m_btn_check{nullptr};         // 忽略按钮
    sw::Button *m_btn_update{nullptr};        // 更新按钮
    sw::Button *m_btn_cancel{nullptr};        // 取消按钮

    Args m_args;                          // 启动参数
    Release m_release;                    // 资源信息
    std::shared_ptr<ClientRequest> m_req; // 网络请求

    void initUI(); // 初始化UI

    void listItemClick() const; // 列表项点击事件

    void btnCheckClicked(); // 忽略按钮点击事件

    void btnUpdateClicked(); // 更新按钮点击事件

    void btnCancelClicked() const; // 取消按钮点击事件

    void getLatestRelease(); // 获取最新的版本

    void checkVersion() const; // 检查是否有新版本

    void downloadRelease(); // 下载更新

    static std::vector<std::wstring> parserLogs(const std::string &log); // 解析日志

    static Asset parserDownloadAsset(const Release &release, bool x64, bool setup); // 解析下载地址

    static void processSoftware(const Asset &asset); // 启动安装包
};