// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QQuickItem>
#include <QRawFont>
#include <QFont>
#include <QFontMetricsF>

#include "objects/server.h"

#include "term.h"

class Screen : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(Server* server READ get_server WRITE set_server);

public:
    Screen(QQuickItem* parent = nullptr);
    ~Screen();

    QFontMetricsF Metrics() const;

    int get_columns() const;

protected:
    void keyPressEvent(QKeyEvent* event) override;

public Q_SLOTS:
    QSGNode* updatePaintNode(QSGNode* old_node, UpdatePaintNodeData*) override;
    
    void on_font_change();
    void update_scroll_height();

    Q_INVOKABLE Server* get_server() { return nullptr; }
    Q_INVOKABLE void set_server(Server* server);

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
