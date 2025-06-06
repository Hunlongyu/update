#include "home.h"

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
    for (auto i = 0; i < 20; ++i)
    {
        m_update_logs->AddItem(L"la la la la la la la la");
    }
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
}

void Home::btnCancelClicked()
{
    m_btn_cancel->Enabled = false;
    std::cout << "cancel clicked" << std::endl;
    sw::App::QuitMsgLoop(0);
}
