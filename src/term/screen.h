// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2020 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QQuickItem>

struct Symbol
{
    char32_t letter;

};

class Screen;

class Text : public QQuickItem
{
    Q_OBJECT

public:
    Text(Screen* parent = nullptr);
    ~Text();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;

public Q_SLOTS:

Q_SIGNALS:

private:
};

class Screen : public QQuickItem
{
    Q_OBJECT

public:
    Screen(QQuickItem* parent = nullptr);
    ~Screen();

public Q_SLOTS:

Q_SIGNALS:

private:
    Text* letter;
};
