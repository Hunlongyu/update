#include "home.h"

Home::Home()
{
    initUI();
    m_req = std::make_shared<ClientRequest>();
    getLatestRelease();
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

    m_btn_check = new sw::Button();
    m_btn_check->Text = L"重新检查";
    m_btn_check->Width = 90;
    m_btn_check->HorizontalAlignment = sw::HorizontalAlignment::Left;
    m_btn_check->RegisterRoutedEvent(sw::ButtonBase_Clicked, [this](sw::UIElement &sender, sw::RoutedEventArgs &e) { btnCheckClicked(); });

    m_btn_update = new sw::Button();
    m_btn_update->Text = L"更新";
    m_btn_update->RegisterRoutedEvent(sw::ButtonBase_Clicked, [this](sw::UIElement &sender, sw::RoutedEventArgs &e) { btnUpdateClicked(); });

    m_btn_cancel = new sw::Button();
    m_btn_cancel->Text = L"取消";
    m_btn_cancel->Margin = sw::Thickness{10, 0, 0, 0};
    m_btn_cancel->RegisterRoutedEvent(sw::ButtonBase_Clicked, [this](sw::UIElement &sender, sw::RoutedEventArgs &e) { btnCancelClicked(); });

    m_btns_grid->AddChild(m_btn_check, sw::GridLayoutTag{0, 0});
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

void Home::btnCheckClicked()
{
    m_btn_check->Enabled = false;
    m_btn_update->Enabled = false;
    getLatestRelease();
}

void Home::btnUpdateClicked()
{
    m_btn_check->Enabled = false;
    m_btn_update->Enabled = false;
    downloadRelease();
}

void Home::btnCancelClicked() const
{
    m_req->cancelDownload();
    // sw::App::QuitMsgLoop(0);
}

void Home::getLatestRelease()
{
    std::thread([this]() {
        const auto resp = m_req->getLatestRelease("https://github.com/Hunlongyu/resting-screen");
        m_release = resp;

        sw::WndBase::Invoke([this, resp]() {
            m_btn_check->Enabled = true;
            m_btn_update->Enabled = true;
            if (resp.tag_name.empty())
            {
                return;
            }
            m_latest_version->Text = L"最新版本：" + sw::Utils::ToWideStr(resp.tag_name, true);
            m_update_logs->Clear();
            if (resp.body.empty())
            {
                m_update_logs->AddItem(L"无更新日志");
                return;
            }
            const auto list = parserLogs(resp.body);
            for (auto log : list)
            {
                m_update_logs->AddItem(log);
            }
        });
    }).detach();
}

void Home::downloadRelease()
{
    std::thread([this]() {
        const auto asset = parserDownloadAsset(m_release, true, false);
        int res = m_req->downloadAsset(asset, [this](size_t downloaded, size_t total) {
            const auto percent = static_cast<uint16_t>(downloaded * 100 / total);
            Invoke([this, percent]() { m_progress_bar->Value = percent; });
        });
        m_btn_update->Enabled = true;
        m_btn_check->Enabled = true;
        if (res == 0)
        {
        }
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
        // 4. 转为 wstring
        auto ws = sw::Utils::ToWideStr(line, true);
        // 5. 去除左右空白
        sw::Utils::Trim(ws);
        if (ws.empty())
            continue;
        result.push_back(ws);
    }
    return result;
}

Asset Home::parserDownloadAsset(const Release &release, bool x64, bool setup)
{
    // C++20 toLower lambda
    auto toLower = [](const std::string &s) {
        std::string r = s;
        std::ranges::transform(r, r.begin(), [](unsigned char c) { return std::tolower(c); });
        return r;
    };

    // 1. 只选.exe结尾的，并且如果x64排除含x86的
    std::vector<const Asset *> exe_assets;
    for (const auto &asset : release.assets)
    {
        auto nameLower = toLower(asset.name);
        if (nameLower.size() < 4 || !nameLower.ends_with(".exe"))
            continue;
        if (x64 && nameLower.find("x86") != std::string::npos)
            continue;
        exe_assets.push_back(&asset);
    }
    if (exe_assets.empty())
        return Asset{}; // 返回空Asset

    // 2. 计算优先级分数
    auto score = [&](const Asset *a) {
        auto name = toLower(a->name);
        int s = 0;
        if (x64 && name.find("x64") != std::string::npos)
            s += 100;
        if (setup && name.find("setup") != std::string::npos)
            s += 10;
        return s;
    };

    // 3. 找分数最高的
    const Asset *best = nullptr;
    int bestScore = -1;
    for (auto asset : exe_assets)
    {
        int s = score(asset);
        if (s > bestScore)
        {
            bestScore = s;
            best = asset;
        }
    }
    return best ? *best : Asset{};
}