// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QQuickItem>
#include <QColor>
#include <QRawFont>
#include <QFont>
#include <QFontMetricsF>

#include <vector>

#include "objects/server.h"

class Screen;

class Text : public QObject
{
    Q_OBJECT

public:
    struct Block {
        std::string text;
        QColor foreground;
        QColor background;
    };

    Text(Screen* parent = nullptr);
    ~Text();

    int line_count() const;

    const std::vector<std::vector<Block>>& get_data() const;

public Q_SLOTS:
    void add_text(QString text);
    void line_recount();

Q_SIGNALS:
    void on_line_count_change();
    void new_text();

private:
    std::vector<std::vector<Block>> data;
    int current_line = 0, current_block = 0, current_char = 0;
    int line_wrap_count = 0;
};

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
    void keyPressEvent(QKeyEvent* event);

public Q_SLOTS:
    QSGNode* updatePaintNode(QSGNode* old_node, UpdatePaintNodeData*);
    
    void on_font_change();
    void update_scroll_height();

    Q_INVOKABLE Server* get_server() { return nullptr; }
    Q_INVOKABLE void set_server(Server* server);

Q_SIGNALS:
    void want_line_recount();

private:
    Text   * letter;
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
