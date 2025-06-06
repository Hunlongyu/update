#include "home.h"
#include "src/request/request.h"

Home::Home()
{
    initUI();
    renderLogs();
}

Home::~Home()
{
}

LRESULT Home::WndProc(const sw::ProcMsg &refMsg)
{
    return Window::WndProc(refMsg);
}

void Home::initUI()
{
    Text = L"程序更新";
    Width = 600;
    Height = 400;
    MaximizeBox = false;
    StartupLocation = sw::WindowStartupLocation::CenterScreen;
    SetLayout<sw::FillLayout>();

    m_grid = new sw::Grid();
    m_grid->SetRows({sw::AutoSizeGridRow{}, sw::AutoSizeGridRow{}, sw::FillRemainGridRow{}, sw::AutoSizeGridRow{}, sw::AutoSizeGridRow{}});
    m_grid->SetColumns({sw::FillRemainGridColumn{}});
    m_grid->Margin = sw::Thickness{10};
    AddChild(m_grid);

    m_stack_panel = new sw::StackPanel();
    m_stack_panel->Orientation = sw::Orientation::Horizontal;
    m_stack_panel->HorizontalAlignment = sw::HorizontalAlignment::Left;
    m_grid->AddChild(m_stack_panel, sw::GridLayoutTag{0, 0});

    m_current_version = new sw::Label();
    m_current_version->Text = L"当前版本：v1.0.1";
    m_stack_panel->AddChild(m_current_version);

    m_latest_version = new sw::Label();
    m_latest_version->Text = L"最新版本：v2.0.0";
    m_latest_version->Margin = sw::Thickness{40, 0, 0, 0};
    m_stack_panel->AddChild(m_latest_version);

    m_logs_tips = new sw::Label();
    m_logs_tips->Text = L"更新日志：";
    m_logs_tips->Margin = sw::Thickness{0, 10, 0, 0};
    m_logs_tips->HorizontalAlignment = sw::HorizontalAlignment::Left;
    m_grid->AddChild(m_logs_tips, sw::GridLayoutTag{1, 0});

    m_update_logs = new sw::ListBox();
    m_update_logs->VerticalAlignment = sw::VerticalAlignment::Stretch;
    m_update_logs->HorizontalAlignment = sw::HorizontalAlignment::Stretch;
    m_update_logs->Margin = sw::Thickness{0, 4, 0, 4};
    m_update_logs->RegisterRoutedEvent(sw::ItemsControl_SelectionChanged, [this](sw::UIElement &sender, sw::RoutedEventArgs &e) { listItemClick(); });
    m_grid->AddChild(m_update_logs, sw::GridLayoutTag{2, 0});

    m_progress_bar = new sw::ProgressBar();
    m_progress_bar->Value = 0;
    m_progress_bar->HorizontalAlignment = sw::HorizontalAlignment::Stretch;
    m_grid->AddChild(m_progress_bar, sw::GridLayoutTag{3, 0});

    m_btns_grid = new sw::Grid();
    m_btns_grid->SetRows({sw::AutoSizeGridRow{}});
    m_btns_grid->SetColumns({sw::FillRemainGridColumn{}, sw::AutoSizeGridColumn{}, sw::AutoSizeGridColumn{}});
    m_btns_grid->HorizontalAlignment = sw::HorizontalAlignment::Stretch;
    m_btns_grid->Margin = sw::Thickness{0, 10, 0, 0};
    m_grid->AddChild(m_btns_grid, sw::GridLayoutTag{4, 0});

    m_btn_ignore = new sw::Button();
    m_btn_ignore->Text = L"忽略该版本";
    m_btn_ignore->Width = 90;
    m_btn_ignore->HorizontalAlignment = sw::HorizontalAlignment::Left;
    m_btn_ignore->RegisterRoutedEvent(sw::ButtonBase_Clicked, [this](sw::UIElement &sender, sw::RoutedEventArgs &e) { btnIgnoreClicked(); });

    m_btn_update = new sw::Button();
    m_btn_update->Text = L"更新";
    m_btn_update->RegisterRoutedEvent(sw::ButtonBase_Clicked, [this](sw::UIElement &sender, sw::RoutedEventArgs &e) { btnUpdateClicked(); });

    m_btn_cancel = new sw::Button();
    m_btn_cancel->Text = L"取消";
    m_btn_cancel->Margin = sw::Thickness{10, 0, 0, 0};
    m_btn_cancel->RegisterRoutedEvent(sw::ButtonBase_Clicked, [this](sw::UIElement &sender, sw::RoutedEventArgs &e) { btnCancelClicked(); });

    m_btns_grid->AddChild(m_btn_ignore, sw::GridLayoutTag{0, 0});
    m_btns_grid->AddChild(m_btn_update, sw::GridLayoutTag{0, 1});
    m_btns_grid->AddChild(m_btn_cancel, sw::GridLayoutTag{0, 2});
}

void Home::renderLogs()
{
}

void Home::listItemClick()
{
    const auto idx = m_update_logs->SelectedIndex.Get();
    if (idx == -1)
    {
        return;
    }
    m_update_logs->SelectedIndex = -1;
}

void Home::btnIgnoreClicked()
{
    m_btn_ignore->Enabled = false;
    std::cout << "ignore clicked" << std::endl;
}

void Home::btnUpdateClicked()
{
    m_btn_update->Enabled = false;
    std::cout << "update clicked" << std::endl;
    getLatestRelease();
    m_btn_update->Enabled = true;
}

void Home::btnCancelClicked()
{
    m_btn_cancel->Enabled = false;
    std::cout << "cancel clicked" << std::endl;
    sw::App::QuitMsgLoop(0);
}

void Home::getLatestRelease()
{
    std::thread([this]() {
        auto req = std::make_shared<ClientRequest>();
        const auto resp = req->getLatestRelease("https://github.com/Hunlongyu/ReadMe");

        if (resp.tag_name.empty())
        {
            return;
        }
        // TODO 这里更新界面UI
        /*if (resp.tag_name.empty())
        {
            return;
        }
        m_latest_version->Text = L"最新版本：" + sw::Utils::ToWideStr(resp.tag_name, true);
        if (resp.body.empty())
        {
            m_update_logs->Clear();
            m_update_logs->AddItem(L"无更新日志");
            return;
        }

        const auto list = parserLogs(resp.body);
        for (auto log : list)
        {
            m_update_logs->AddItem(log);
        }*/

    }).detach();
}

std::vector<std::wstring> Home::parserLogs(const std::string &log)
{
    std::vector<std::wstring> result;

    std::string new_log = log;
    size_t pos = 0;
    while ((pos = new_log.find("\r\n", pos)) != std::string::npos)
    {
        new_log.replace(pos, 2, "\n");
        ++pos;
    }

    // 2. 拆分每一行
    std::istringstream iss(new_log);
    std::string line;

    while (std::getline(iss, line, '\n'))
    {
        // 3. 跳过包含“更新日志”的行
        if (line.find("更新日志") != std::string::npos)
            continue;
        // 4. 去除左右空白
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);
        if (line.empty())
            continue;
        // 5. 转为wstring
        const auto ws = sw::Utils::ToWideStr(line, true);
        result.push_back(ws);
    }
    return result;
}
