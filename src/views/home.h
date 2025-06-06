#pragma once

#include <SimpleWindow.h>

class Home : public sw::Window
{
  public:
    Home();
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
    sw::Button *m_btn_ignore{nullptr};        // 忽略按钮
    sw::Button *m_btn_update{nullptr};        // 更新按钮
    sw::Button *m_btn_cancel{nullptr};        // 取消按钮

    void initUI(); // 初始化UI

    void renderLogs(); // 渲染日志

    void listItemClick();

    void btnIgnoreClicked(); // 忽略按钮点击事件

    void btnUpdateClicked(); // 更新按钮点击事件

    void btnCancelClicked(); // 取消按钮点击事件
};