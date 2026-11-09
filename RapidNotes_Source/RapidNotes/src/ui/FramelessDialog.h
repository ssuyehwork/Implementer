#ifndef FRAMELESSDIALOG_H
#define FRAMELESSDIALOG_H

#include "FramelessDialogBase.h"
#include "dialogs/FramelessInputDialog.h"
#include "dialogs/FramelessColorPicker.h"
#include "dialogs/FramelessConfirmDialog.h"
#include "dialogs/FramelessMessageBox.h"
#include "BatchProgressDialog.h"

// 全局导出命名空间，修复所有派生窗口的 C2504 基类未定义错误
using RapidNotes::FramelessDialog;
using FramelessProgressDialog = RapidNotes::BatchProgressDialog;

#endif // FRAMELESSDIALOG_H
