#include "lds_messagebox.h"
#include <QTimeLine>
#include "lds_toolbar_button.h"
#include <QVariant>
#include <QDialog>
#include <QMovie>
#include <QAbstractButton>
#include <QHBoxLayout>
#include<QLabel>
#include <QtDebug>
#include <QTimer>
#include <QEventLoop>
#include <QStyle>
#include <QTextEdit>
#include <QCoreApplication>
#include <QScrollBar>
#include <QTime>

lds_messagebox::lds_messagebox(QObject *parent) :
    QObject(parent)
{
}

int lds_messagebox::warning(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Warning, title, text,
                                             button0Text, button1Text, button2Text, "", delayclose);
}

int lds_messagebox::warning(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Warning, title, text,
                                             "", "", "", "", delayclose);
}

int lds_messagebox::information(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Information, title, text,
                                             button0Text, button1Text, button2Text, "", delayclose);
}

int lds_messagebox::information(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Information, title, text, "", "", "", "", delayclose);
}

int lds_messagebox::question(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, const QString &button3Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Question, title, text,
                                             button0Text, button1Text, button2Text,button3Text ,delayclose);

}

int lds_messagebox::question(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Question, title, text, "", "", "", "", delayclose);
}

int lds_messagebox::critical(QWidget *parent, const QString &title, const QString &text, const QString &button0Text, const QString &button1Text, const QString &button2Text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Critical, title, text,
                                             button0Text, button1Text, button2Text,"", delayclose);

}

int lds_messagebox::critical(QWidget *parent, const QString &title, const QString &text, bool delayclose)
{
    return lds_messagebox::showOldMessageBox(parent, QMessageBox::Critical, title, text, "", "", "", "", delayclose);
}

int lds_messagebox::showOldMessageBox(QWidget *parent, QMessageBox::Icon icon, const QString &title, const QString &text, QString button0Text, const QString &button1Text, const QString &button2Text, const QString &button3Text, bool delayclose)
{
    while(parent && parent->parentWidget()) {
        if(parent->isTopLevel()) break;
        parent = parent->parentWidget();
    }
    if(button0Text.isEmpty())button0Text=tr("确定");

    if(text.length() > 100) {
        QStringList btn_list;
        if(!button0Text.isEmpty()) btn_list << button0Text;
        if(!button1Text.isEmpty()) btn_list << button1Text;
        if(!button2Text.isEmpty()) btn_list << button2Text;
        if(!button3Text.isEmpty()) btn_list << button3Text;
        lds_messagebox_textedit dialog(parent, title, text, btn_list);
        return lds_roundeddialog_rect_align(&dialog, false).exec();
    }
    lds_msgbox messageBox(icon, title, text, QMessageBox::NoButton, parent);
    {
        lds_toolbar_button *b=new lds_toolbar_button;
        b->setProperty("outer_stylesheet", "pushbutton");
        b->setText(button0Text);
        messageBox.addButton(b, QMessageBox::ActionRole);
        messageBox.btnok=b;
    }
    if(!button1Text.isEmpty()){
        lds_toolbar_button *b=new lds_toolbar_button;
        b->setProperty("outer_stylesheet", "pushbutton");
        b->setText(button1Text);
        messageBox.addButton(b, QMessageBox::ActionRole);
    }
    if(!button2Text.isEmpty()){
        lds_toolbar_button *b=new lds_toolbar_button;
        b->setProperty("outer_stylesheet", "pushbutton");
        b->setText(button2Text);
        messageBox.addButton(b, QMessageBox::ActionRole);
    }
    if(!button3Text.isEmpty()){
        lds_toolbar_button *b=new lds_toolbar_button;
        b->setProperty("outer_stylesheet", "pushbutton");
        b->setText(button3Text);
        messageBox.addButton(b, QMessageBox::ActionRole);
    }

    QTimeLine timeLine(5000, 0);
    if(delayclose){
        timeLine.setFrameRange(0, 5);
        timeLine.setDirection(QTimeLine::Backward);
        connect(&timeLine, SIGNAL(frameChanged(int)), &messageBox, SLOT(updateTimeframe(int)));
        connect(&messageBox,SIGNAL(btnok_finished(int)),&messageBox, SLOT(done(int)));
        timeLine.start();
    }

    messageBox.resize(messageBox.sizeHint());
    return  lds_roundeddialog_rect_align(&messageBox, MESSAGE_TITLE_VOID != title).exec();
}

void lds_msgbox::updateText(const QString &text)
{
    this->setText(text);
}

void lds_msgbox::updateTimeframe(int index)
{
    btnok->setText(btnok->text().split(":").value(0)+":"+QString::number(index));
    if(index==0) emit btnok_finished(0);
}

void lds_msgbox::closeEvent(QCloseEvent *e)
{
    QMessageBox::closeEvent(e);
}

lds_messagebox_loading::lds_messagebox_loading(QWidget *parent, const QString &title, const QString &text)
    : QDialog(parent)
{
    this->setWindowTitle(title);

    mv = new QMovie(this);
    mv->setFileName(":/image/loading.gif");
    label_image = new QLabel(this);
    label_image->setFixedSize(40, 40);
    label_image->setScaledContents(true);
    label_image->setMovie(mv);
    label_text = new QLabel(this);
    QHBoxLayout*hlayout = new QHBoxLayout;
    hlayout->addWidget(label_image);
    hlayout->addWidget(label_text, 1);
    this->setLayout(hlayout);

    label_text->setText(text);
    mv->start();
    this->setFixedHeight(60);
}

lds_messagebox_loading::~lds_messagebox_loading()
{
}

void lds_messagebox_loading::image_dynamic_run()
{
    mv->jumpToNextFrame();
    label_image->repaint();
    this->repaint();
}

QString lds_messagebox_loading::text()
{
    return label_text->text();
}

void lds_messagebox_loading::setText(const QString &text)
{
    label_text->setText(text);
    label_text->repaint();
    this->repaint();
}

void lds_messagebox_loading::update_image()
{

}


lds_messagebox_loading_show::lds_messagebox_loading_show(QWidget *parent, const QString &title, const QString &text)
    : QObject(parent)
{
    loading = new lds_messagebox_loading(parent, title, text);
    dialog = new lds_roundeddialog_rect_align(loading, false);
}

lds_messagebox_loading_show::~lds_messagebox_loading_show()
{
    delete dialog;
}

void lds_messagebox_loading_show::show()
{
    dialog->show();
}

void lds_messagebox_loading_show::hide()
{
    dialog->hide();
}

lds_roundeddialog_rect_align *lds_messagebox_loading_show::widget()
{
    return dialog;
}

void lds_messagebox_loading_show::show_delay()
{
    show();
    QEventLoop eloop;
    QTimer::singleShot(100, &eloop, SLOT(quit()));
    eloop.exec();
}

//delay 是 倒计时的问题
void lds_messagebox_loading_show::setText(const QString &text, int delay)
{
    setText(text);
    if(delay > 0) {
        bool first = true;
        QTime dieTime = QTime::currentTime().addMSecs(delay);
        while( QTime::currentTime() < dieTime ){
            int remain_secs = QTime::currentTime().msecsTo(dieTime)/1000;
            if(remain_secs > 0)
                loading->setText(text+QString("(%1)").arg(remain_secs));
            else
                loading->setText(text);

            if(first) {
                dialog->buddyResize();
                first = false;
            }
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
}

void lds_messagebox_loading_show::setText(const QString &text)
{
    loading->setText(text);
    dialog->buddyResize();
}

void lds_messagebox_loading_show::image_dynamic_run()
{
    loading->image_dynamic_run();
}

QString lds_messagebox_loading_show::text()
{
    return loading->text();
}

void lds_messagebox_loading_show::updateProgress(qint64 bytesStep, qint64 bytesTotal)
{
    loading->setText(QString("%1/%2").arg(bytesStep).arg(bytesTotal));
    dialog->buddyResize();
}

lds_messagebox_textedit::lds_messagebox_textedit(QWidget *parent, const QString &title, const QString &text, QStringList buttonTexts)
    : QDialog(parent)
{
    QLabel *label_title = new QLabel(this);
    QLabel *label_icon = new QLabel(this);
    QTextEdit *edit = new QTextEdit(this);
    QHBoxLayout *hlayout;
    QVBoxLayout *vlayout = new QVBoxLayout;
    edit->setReadOnly(true);

    //icon_title
    hlayout = new QHBoxLayout;
    hlayout->addWidget(label_icon);
    hlayout->addWidget(label_title, 1);
    vlayout->addLayout(hlayout);

    //edit
    vlayout->addWidget(edit, 1);

    //btn
    hlayout = new QHBoxLayout;
    hlayout->addStretch(1);
    if(buttonTexts.isEmpty())  buttonTexts << tr("确定");
    foreach(const QString btn, buttonTexts) {
        lds_toolbar_button *p=new lds_toolbar_button(this);
        p->setProperty("outer_stylesheet", "pushbutton");
        p->setText(btn);
        hlayout->addWidget(p);
        btnlist.append(p);
        connect(p, SIGNAL(clicked()), this, SLOT(todone()));
    }
    vlayout->addLayout(hlayout);

    this->setLayout(vlayout);

    label_title->setText(title);
    label_icon->setPixmap(
                style()->standardIcon(QStyle::SP_MessageBoxWarning, 0, this).pixmap(30, 30)
                );
    edit->setText(text);

    this->setFixedSize(400, 300);//大小不确定的话，垂直滚动条的位置可能会变
}

lds_messagebox_textedit::~lds_messagebox_textedit()
{

}

void lds_messagebox_textedit::todone()
{
    this->done(btnlist.indexOf(this->sender()));
}
