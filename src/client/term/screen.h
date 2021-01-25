// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QQuickPaintedItem>
#include <QRawFont>
#include <QFont>
#include <QFontMetricsF>

#include "objects/server.h"

#include "term.h"

class Screen : public QQuickPaintedItem
{
    Q_OBJECT

    Q_PROPERTY(Pty* server READ get_server WRITE set_server);

public:
    Screen(QQuickItem* parent = nullptr);
    ~Screen();

    QFontMetricsF Metrics() const;

    int get_columns() const;
    int get_rows() const;

protected:
    void keyPressEvent(QKeyEvent* event) override;

public Q_SLOTS:
    void paint(QPainter *painter) override;

    void on_font_change();
    void update_scroll_height();

    Q_INVOKABLE Pty* get_server() { return nullptr; }
    Q_INVOKABLE void set_server(Pty* server);

Q_SIGNALS:
    void want_line_recount();

private:
    Term   * letter;
    Pty    * pty        = nullptr;
    QThread* pty_thread;
    struct {
        QFont    base;
        QRawFont normal;
        QRawFont bold;
        QRawFont italic;
        QRawFont bold_italic;
    } font;
};
