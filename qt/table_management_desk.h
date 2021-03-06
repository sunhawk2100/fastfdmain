#ifndef TABLE_MANAGEMENT_DESK_H
#define TABLE_MANAGEMENT_DESK_H

#include <QGraphicsItem>
#include "lds_bool.h"
class table_management_pad;
#define deskView qobject_cast<table_management_graphicsview *>(this->scene()->views()[0])

class table_management_desk : public QGraphicsItem
{
public:
    enum shapeType{
        shape_rectangle_min = 0,
        shape_triangle,
        shape_circle,
        shape_max
    };
    enum dataType{
        item_hscale_min = 0,
        item_vscale,

        item_rotate,
        item_boundingRect,
        item_ch_areano,
        item_ch_tableno,

        item_shapetype,
        item_pos,//不做实时记录，每次保存前，另行处理

        item_max
    };

    static int scale_add_value;
    static int rotate_add_value;
public:
    table_management_desk(table_management_pad *pad, shapeType stype = shape_rectangle_min);
    ~table_management_desk();
    QRectF boundingRect() const ;
    QPainterPath shape() const ;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget */*widget*/) ;

    enum { Type = 2 };
    virtual int type() const;

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void focusOutEvent(QFocusEvent *event);


    void set_shapetype(shapeType stype);
    shapeType get_shapetype();

    void update_boundrect(int type, int add_sub_key);
    void updaste_textcolor(QColor textcolor);

    QString get_text();
    void paint_cell(const QString &ch_billnos,
                    QMap<QString/*ch_tableno*/, bool> *desk_preprint_map,
                    float num_cost,
                    const QString &order_tableno/*是否预定*/,
                    const QString &vch_tablename,
                    const QString &div_bar);
    void paint_path(QPainter *painter, const QPainterPath &path, const QPixmap &pixmap);
    void paint_text(QPainter *painter, const QRect &rect, const QStyleOptionGraphicsItem *item, const QString &text);

private:
    bool ismove;
    table_management_pad *pad;
    QColor textcolor;

    ////////
public:
private:
    QString ch_billnos;
    QMap<QString/*ch_tableno*/, bool> *desk_preprint_map;
    float num_cost;
    QString  order_tableno;//已经预定
    QString pay_unit_prefix;
    QString div_bar;

};


#endif // TABLE_MANAGEMENT_DESK_H
