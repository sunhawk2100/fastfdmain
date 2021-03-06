#include "takeout_cook.h"
#include <QDateTime>
#include "n_func.h"
#include "lds_messagebox.h"
#include "lds_query.h"
#include <QDebug>
#include "n_func.h"
#include <QSqlError>
#include <QSqlError>
#include "backheader.h"
#include "frontviewdialog_virtual.h"
#include "public_sql.h"
#include "w_scr_dish_select_operid.h"
#include "w_bt_dish.h"
#include "w_scr_dish_pay_widget.h"
#include "public_printby_ch_billno.h"
#include "w_scr_dish_restaurant_div_pop.h"
#include "w_scr_dish_main.h"

takeout_cook::takeout_cook(const fexpandmain_model_sqltablemodel_data *tablemodel, const QString &ch_tableno, QWidget *parent)
    : QDialog(parent),
      ui(new Ui::takeout_cook)
{
    ui->setupUi(this);
    this->ch_tableno = ch_tableno;
    //1
    ui->lineEdit->setEnabled(false);
    //    ui->lineEdit_consum->setReadOnly(true);
    //    ui->lineEdit_discount->setReadOnly(true);
    //    ui->lineEdit_int_rate->setReadOnly(true);
    //    ui->lineEdit_present->setReadOnly(true);
    //    ui->lineEdit_blotout->setReadOnly(true);
    //    ui->lineEdit_realreceive->setReadOnly(true);

    ui->lineEdit_sendman->setEnabled(false);
    ui->lineEdit_operid->setEnabled(false);
    //2
    ui->lineEdit_consum->setValue(tablemodel->getKeepdata().consum);
    ui->lineEdit_present->setValue(tablemodel->getKeepdata().present); ui->lineEdit_present->enableShowNegativeValue();
    ui->lineEdit_discount->setValue(tablemodel->getKeepdata().discount);ui->lineEdit_discount->enableShowNegativeValue();
    ui->lineEdit_int_rate->setValue(tablemodel->getKeepdata().num_rate);

    float v = ui->lineEdit_consum->value() - ui->lineEdit_discount->value() - ui->lineEdit_present->value() + ui->lineEdit_int_rate->value();
    ui->lineEdit_realreceive->setValue(w_scr_dish_pay_widget::valuerountbysys(v));
    ui->lineEdit_blotout->setValue(v - ui->lineEdit_realreceive->value());ui->lineEdit_blotout->enableShowNegativeValue();

    ui->lineEdit_operid->setText(public_sql::gs_operid);//操作员
    ui->lineEdit_sendtime->setText(n_func::f_get_sysdatetime().toString("yyyy-MM-dd hh:mm:ss"));

    if(lds::sysconf->value("system_setting/waimaidef0000", false).toBool()){
        ui->lineEdit_sendman->setText("0000");
    }
    _tablemodel = tablemodel;
    connect(ui->pushButton_ok, SIGNAL(clicked()),this,SLOT(took()));
    connect(ui->pushButton_cancel, SIGNAL(clicked()),this,SLOT(tocancel()));
    connect(ui->pushButton_sendman, SIGNAL(clicked()),this,SLOT(tosendman()));
}

takeout_cook::~takeout_cook()
{
    delete ui;
}

void takeout_cook::took()
{
    if(ui->lineEdit_tel->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->label_tel->text() + "," + tr("信息不完整,请核实"));
        return;
    }
    if(ui->lineEdit_orderman->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->label_orderman->text() + "," + tr("信息不完整,请核实"));
        return;
    }
    if(ui->lineEdit_sendaddress->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->label_sendaddress->text() + "," + tr("信息不完整,请核实"));
        return;
    }
    if(ui->lineEdit_sendman->text().trimmed().isEmpty()) {
        lds_messagebox::warning(this, MESSAGE_TITLE_VOID, ui->label_sendman->text() + "," + tr("信息不完整,请核实"));
        return;
    }
    if(1==lds_messagebox::question(this, MESSAGE_TITLE_VOID,tr("确认派送?"), tr("确认"), tr("取消"))){
        return;
    }

    {
        lds_query::tran_saction();
        lds_query query;
        QString  errstring;
        //guestid
        QDateTime curdatetime = n_func::f_get_sysdatetime();
        QString ch_guestid_left7 = "C" + curdatetime.toString("yyMMdd");
        query.execSelect(QString("select Max(ch_guestid) from cey_u_togo_guest where Left(ch_guestid,7) = '%1' ").arg(ch_guestid_left7));
        query.next();
        QString guestid = query.recordValue(0).toString();
        guestid = ch_guestid_left7 + "-" + QString().sprintf("%04d", guestid.right(4).toInt()+1);

        //ch_togono
        QString ch_togono_left7 = "W" + curdatetime.toString("yyMMdd");
        query.execSelect(QString("select Max(right(ch_togono, 5)) from cey_u_togo where Left(ch_togono,7) = '%1' ").arg(ch_togono_left7));
        query.next();
        QString ch_togono = query.recordValue(0).toString();
        ch_togono = ch_togono_left7 + "-" + QString().sprintf("%05d", ch_togono.toInt()+1);

        if(this->ch_tableno == w_scr_dish_main::Q000) {
            if(!w_scr_dish_restaurant_div_pop::cey_u_master_insert(_tablemodel->cur_ch_billno())) {
                goto rollback;
            }
            //cey_u_table
            if(!query.execInsert("cey_u_table",qrtVariantPairList()

                                 << qrtVariantPair("ch_billno", _tablemodel->cur_ch_billno()) << qrtVariantPair("ch_tableno", ch_tableno)<< qrtVariantPair("int_person", 1) << qrtVariantPair("ch_state", 1) << qrtVariantPair("vch_operID", public_sql::gs_operid)
                                 << qrtVariantPair("dt_operdate", ui->lineEdit_sendtime->text())<< qrtVariantPair("ch_order_state", 5)<< qrtVariantPair("vch_waiter", "")<< qrtVariantPair("vch_memo", ""))) {
                goto rollback;
            }
        }

        //cey_u_orderdish
        if(!_tablemodel->try_commit_data(errstring)){
            goto rollback;
        }
        //充值外卖标志位
        if(!query.execUpdate("cey_u_orderdish_print_tr",
                             "vch_plan_k3o3_state", QString(6, w_bt_dish::k2o3_state2hex_char(w_bt_dish::KITCHEN_WAIMAI)),
                             QString("ch_billno = '%2' ").arg(_tablemodel->cur_ch_billno())
                             )) {
            goto rollback;
        }
        if(!query.execUpdate("cey_u_orderdish",qrtVariantPairList()
                             << qrtVariantPair("ch_togono", ch_togono),
                             QString("ch_billno = '%2' ").arg(_tablemodel->cur_ch_billno())
                             )) {
            goto rollback;
        }
        //cey_u_togo_guest
        if(!query.execInsert("cey_u_togo_guest",qrtVariantPairList()
                             << qrtVariantPair("ch_guestid", guestid)<< qrtVariantPair("vch_guestname", ui->lineEdit_orderman->text())<< qrtVariantPair("vch_tel", ui->lineEdit_tel->text())<< qrtVariantPair("vch_address", ui->lineEdit_sendaddress->text())
                             )){
            goto rollback;
        }
        //cey_u_togo
        if(!query.execInsert("cey_u_togo",qrtVariantPairList()

                             << qrtVariantPair("ch_togono", ch_togono)
                             << qrtVariantPair("ch_billno", _tablemodel->cur_ch_billno())
                             << qrtVariantPair("num_cost", ui->lineEdit_consum->value())
                             << qrtVariantPair("num_discount", ui->lineEdit_discount->value())
                             << qrtVariantPair("num_present", ui->lineEdit_present->value())

                             << qrtVariantPair("num_blotout", ui->lineEdit_blotout->value())
                             << qrtVariantPair("num_rate", ui->lineEdit_int_rate->value())
                             << qrtVariantPair("vch_waiter", ui->lineEdit_sendman->text())
                             << qrtVariantPair("vch_address", ui->lineEdit_sendaddress->text())
                             << qrtVariantPair("vch_linkman", ui->lineEdit_orderman->text())

                             << qrtVariantPair("vch_tel", ui->lineEdit_tel->text())
                             << qrtVariantPair("ch_state", "N")
                             << qrtVariantPair("vch_operID", public_sql::gs_operid)
                             << qrtVariantPair("dt_operdate", ui->lineEdit_sendtime->text())
                             )) {
            goto rollback;
        }
    }
    //01->12
    lds_query::com_mit();
    {
        public_printby_ch_billno printer(_tablemodel->cur_ch_billno());
        printer.toprintwaimai(printer.defpath());
        public_printby_ch_billno::print_label(_tablemodel->cur_ch_billno());
    }
    lds_messagebox::information(this, MESSAGE_TITLE_VOID,tr("外卖已制作派送"));
    this->accept();
    return;
rollback:
    lds_query::roll_back();
    lds_messagebox::warning(this, MESSAGE_TITLE_VOID,tr("外卖制作派送失败"));
}

void takeout_cook::tocancel()
{
    this->reject();
}

void takeout_cook::tosendman()
{
    w_scr_dish_select_operid dialog(this);
    dialog.setWindowTitle(tr("操作员"));
    dialog.setOperid(ui->lineEdit_sendman->text());
    if(QDialog::Accepted == lds_roundeddialog_rect_align(&dialog).exec())
    {
        ui->lineEdit_sendman->setText(dialog.selectOperid());
    }
}
