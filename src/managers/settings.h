// SPDX-License-Identifier: GPL-3.0-or-later
// SPDX-FileCopyrightText: 2021 Jared Irwin <jrairwin@sympatico.ca>

#pragma once

#include <QObject>
#include <QSettings>
#include <QFont>

#include "term/term.h"

class Settings : public QObject
{
    Q_OBJECT
    Settings();

private:
    Q_PROPERTY(int         wol_port            READ get_wol_port            WRITE set_wol_port            NOTIFY changed_wol_port           )
    Q_PROPERTY(double      server_refresh_rate READ get_server_refresh_rate WRITE set_server_refresh_rate NOTIFY changed_server_refresh_rate)
    Q_PROPERTY(QFont       font                READ get_font                WRITE set_font                NOTIFY changed_font               )
    Q_PROPERTY(Term::Types term_type           READ get_term_type           WRITE set_term_type           NOTIFY changed_term_type          )

public:
    static Settings& Instance();

public Q_SLOTS:
    Q_INVOKABLE uint16_t    get_wol_port           ();
    Q_INVOKABLE double      get_server_refresh_rate();
    Q_INVOKABLE QFont       get_font               ();
    Q_INVOKABLE Term::Types get_term_type          ();

    Q_INVOKABLE void set_wol_port           (uint16_t    value);
    Q_INVOKABLE void set_server_refresh_rate(double      value);
    Q_INVOKABLE void set_font               (QFont       value);
    Q_INVOKABLE void set_term_type          (Term::Types value);

Q_SIGNALS:
    void changed_wol_port           ();
    void changed_server_refresh_rate();
    void changed_font               ();
    void changed_term_type          ();

private:
    QSettings settings;
};

using SettingsPointer = Settings*;
Q_DECLARE_METATYPE(SettingsPointer);