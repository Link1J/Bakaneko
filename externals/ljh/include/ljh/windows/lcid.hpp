
//          Copyright Jared Irwin 2020
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

// lcid.hpp - v1.0
// SPDX-License-Identifier: BSL-1.0
// 
// Requires C++17
// 
// ABOUT
//
// USAGE
//
// Version History
//     1.0 Inital Version

#pragma once
#define NOMINMAX
#include <windows.h>
#include <limits>

#undef min
#undef max

namespace ljh::windows
{
	enum class lcid : LCID
	{
		none = 0,
		unknown = std::numeric_limits<LCID>::max(),

		af_ZA  = 0x0436, // Afrikaans
		sq_AL  = 0x041c, // Albanian
		gsw_FR = 0x0484, // Alsatian - France
		am_ET  = 0x045e, // Amharic - Ethiopia
		ar_SA  = 0x0401, // Arabic - Saudi Arabia
		ar_IQ  = 0x0801, // Arabic - Iraq
		ar_EG  = 0x0c01, // Arabic - Egypt
		ar_LY  = 0x1001, // Arabic - Libya
		ar_DZ  = 0x1401, // Arabic - Algeria
		ar_MA  = 0x1801, // Arabic - Morocco
		ar_TN  = 0x1c01, // Arabic - Tunisia
		ar_OM  = 0x2001, // Arabic - Oman
		ar_YE  = 0x2401, // Arabic - Yemen
		ar_SY  = 0x2801, // Arabic - Syria
		ar_JO  = 0x2c01, // Arabic - Jordan
		ar_LB  = 0x3001, // Arabic - Lebanon
		ar_KW  = 0x3401, // Arabic - Kuwait
		ar_AE  = 0x3801, // Arabic - United Arab Emirates
		ar_BH  = 0x3c01, // Arabic - Bahrain
		ar_QA  = 0x4001, // Arabic - Qatar
		hy_AM  = 0x042b, // Armenian
		as_IN  = 0x044d, // Assamese - India
		az_AZ  = 0x042c, // Azeri - Latin
		//az_AZ  = 0x082c, // Azeri - Cyrillic
		ba_RU  = 0x046d, // Bashkir
		eu_ES  = 0x042d, // Basque - Russia
		be_BY  = 0x0423, // Belarusian
		bn_IN  = 0x0445, // Begali
		//bs_BA  = 0x201a, // Bosnian - Cyrillic
		bs_BA  = 0x141a, // Bosnian - Latin
		br_FR  = 0x047e, // Breton - France
		bg_BG  = 0x0402, // Bulgarian
		ca_ES  = 0x0403, // Catalan
		zh_CHS = 0x0004, // Chinese - Simplified
		zh_TW  = 0x0404, // Chinese - Taiwan
		zh_CN  = 0x0804, // Chinese - PRC
		zh_HK  = 0x0c04, // Chinese - Hong Kong S.A.R.
		zh_SG  = 0x1004, // Chinese - Singapore
		zh_MO  = 0x1404, // Chinese - Macao S.A.R.
		zh_CHT = 0x7c04, // Chinese - Traditional
		co_FR  = 0x0483, // Corsican - France
		hr_HR  = 0x041a, // Croatian
		hr_BA  = 0x101a, // Croatian - Bosnia
		cs_CZ  = 0x0405, // Czech
		da_DK  = 0x0406, // Danish
		gbz_AF = 0x048c, // Dari - Afghanistan
		div_MV = 0x0465, // Divehi - Maldives
		nl_NL  = 0x0413, // Dutch - The Netherlands
		nl_BE  = 0x0813, // Dutch - Belgium
		en_US  = 0x0409, // English - United States
		en_GB  = 0x0809, // English - United Kingdom
		en_AU  = 0x0c09, // English - Australia
		en_CA  = 0x1009, // English - Canada
		en_NZ  = 0x1409, // English - New Zealand
		en_IE  = 0x1809, // English - Ireland
		en_ZA  = 0x1c09, // English - South Africa
		en_JA  = 0x2009, // English - Jamaica
		en_CB  = 0x2409, // English - Caribbean
		en_BZ  = 0x2809, // English - Belize
		en_TT  = 0x2c09, // English - Trinidad
		en_ZW  = 0x3009, // English - Zimbabwe
		en_PH  = 0x3409, // English - Philippines
		en_IN  = 0x4009, // English - India
		en_MY  = 0x4409, // English - Malaysia
		//en_IN  = 0x4809, // English - Singapore
		et_EE  = 0x0425, // Estonian
		fo_FO  = 0x0438, // Faroese
		fil_PH = 0x0464, // Filipino
		fi_FI  = 0x040b, // Finnish
		fr_FR  = 0x040c, // French - France
		fr_BE  = 0x080c, // French - Belgium
		fr_CA  = 0x0c0c, // French - Canada
		fr_CH  = 0x100c, // French - Switzerland
		fr_LU  = 0x140c, // French - Luxembourg
		fr_MC  = 0x180c, // French - Monaco
		fy_NL  = 0x0462, // Frisian - Netherlands
		gl_ES  = 0x0456, // Galician
		ka_GE  = 0x0437, // Georgian
		de_DE  = 0x0407, // German - Germany
		de_CH  = 0x0807, // German - Switzerland
		de_AT  = 0x0c07, // German - Austria
		de_LU  = 0x1007, // German - Luxembourg
		de_LI  = 0x1407, // German - Liechtenstein
		el_GR  = 0x0408, // Greek
		kl_GL  = 0x046f, // Greenlandic - Greenland
		gu_IN  = 0x0447, // Gujarati
		ha_NG  = 0x0468, // Hausa - Latin
		he_IL  = 0x040d, // Hebrew
		hi_IN  = 0x0439, // Hindi
		hu_HU  = 0x040e, // Hungarian
		is_IS  = 0x040f, // Icelandic
		id_ID  = 0x0421, // Indonesian
		iu_CA  = 0x045d, // Inuktitut - Syllabics
		//iu_CA  = 0x085d, // Inuktitut - Latin
		ga_IE  = 0x083c, // Irish - Ireland
		it_IT  = 0x0410, // Italian - Italy
		it_CH  = 0x0810, // Italian - Switzerland
		ja_JP  = 0x0411, // Japanese
		kn_IN  = 0x044b, // Kannada - India
		kk_KZ  = 0x043f, // Kazakh
		kh_KH  = 0x0453, // Khmer - Cambodia
		qut_GT = 0x0486, // K'iche - Guatemala
		rw_RW  = 0x0487, // Kinyarwanda - Rwanda
		kok_IN = 0x0457, // Konkani
		ko_KR  = 0x0412, // Korean
		ky_KG  = 0x0440, // Kyrgyz
		lo_LA  = 0x0454, // Lao - Lao PDR
		lv_LV  = 0x0426, // Latvian
		lt_LT  = 0x0427, // Lithuanian
		dsb_DE = 0x082e, // Lower Sorbian - Germany
		lb_LU  = 0x046e, // Luxembourgish
		mk_MK  = 0x042f, // FYROM Macedonian
		ms_MY  = 0x043e, // Malay - Malaysia
		ms_BN  = 0x083e, // Malay - Brunei Darussalam
		ml_IN  = 0x044c, // Malayalam - India
		mt_MT  = 0x043a, // Maltese
		mi_NZ  = 0x0481, // Maori
		arn_CL = 0x047a, // Mapudungun
		mr_IN  = 0x044e, // Marathi
		moh_CA = 0x047c, // Mohawk - Canada
		mn_MN  = 0x0450, // Mongolian - Cyrillic
		mn_CN  = 0x0850, // Mongolian - PRC
		ne_NP  = 0x0461, // Nepali
		nb_NO  = 0x0414, // Norwegian - Bokmal
		nn_NO  = 0x0814, // Norwegian - Nynorsk
		oc_FR  = 0x0482, // Occitan - France
		or_IN  = 0x0448, // Oriya - India
		ps_AF  = 0x0463, // Pashto - Afghanistan
		fa_IR  = 0x0429, // Persian
		pl_PL  = 0x0415, // Polish
		pt_BR  = 0x0416, // Portuguese - Brazil
		pt_PT  = 0x0816, // Portuguese - Portugal
		pa_IN  = 0x0446, // Punjabi
		quz_BO = 0x046b, // Quechua (Bolivia)
		quz_EC = 0x086b, // Quechua (Ecuador)
		quz_PE = 0x0c6b, // Quechua (Peru)
		ro_RO  = 0x0418, // Romanian - Romania
		rm_CH  = 0x0417, // Romansh
		ru_RU  = 0x0419, // Russian
		smn_FI = 0x243b, // Sami Finland
		smj_NO = 0x103b, // Sami Norway
		smj_SE = 0x143b, // Sami Sweden
		se_NO  = 0x043b, // Sami Northern Norway
		se_SE  = 0x083b, // Sami Northern Sweden
		se_FI  = 0x0c3b, // Sami Northern Finland
		sms_FI = 0x203b, // Sami Skolt
		sma_NO = 0x183b, // Sami Southern Norway
		sma_SE = 0x1c3b, // Sami Southern Sweden
		sa_IN  = 0x044f, // Sanskrit
		//sr_SP  = 0x0c1a, // Serbian - Cyrillic
		//sr_BA  = 0x1c1a, // Serbian - Bosnia Cyrillic
		sr_SP  = 0x081a, // Serbian - Latin
		sr_BA  = 0x181a, // Serbian - Bosnia Latin
		si_LK  = 0x045b, // Sinhala - Sri Lanka
		ns_ZA  = 0x046c, // Northern Sotho
		tn_ZA  = 0x0432, // Setswana - Southern Africa
		sk_SK  = 0x041b, // Slovak
		sl_SI  = 0x0424, // Slovenian
		es_ES  = 0x040a, // Spanish - Spain
		es_MX  = 0x080a, // Spanish - Mexico
		//es_ES  = 0x0c0a, // Spanish - Spain (Modern)
		es_GT  = 0x100a, // Spanish - Guatemala
		es_CR  = 0x140a, // Spanish - Costa Rica
		es_PA  = 0x180a, // Spanish - Panama
		es_DO  = 0x1c0a, // Spanish - Dominican Republic
		es_VE  = 0x200a, // Spanish - Venezuela
		es_CO  = 0x240a, // Spanish - Colombia
		es_PE  = 0x280a, // Spanish - Peru
		es_AR  = 0x2c0a, // Spanish - Argentina
		es_EC  = 0x300a, // Spanish - Ecuador
		es_CL  = 0x340a, // Spanish - Chile
		es_UR  = 0x380a, // Spanish - Uruguay
		es_PY  = 0x3c0a, // Spanish - Paraguay
		es_BO  = 0x400a, // Spanish - Bolivia
		es_SV  = 0x440a, // Spanish - El Salvador
		es_HN  = 0x480a, // Spanish - Honduras
		es_NI  = 0x4c0a, // Spanish - Nicaragua
		es_PR  = 0x500a, // Spanish - Puerto Rico
		es_US  = 0x540a, // Spanish - United States
		sw_KE  = 0x0441, // Swahili
		sv_SE  = 0x041d, // Swedish - Sweden
		sv_FI  = 0x081d, // Swedish - Finland
		syr_SY = 0x045a, // Syriac
		tg_TJ  = 0x0428, // Tajik - Cyrillic
		tmz_DZ = 0x085f, // Tamazight - Latin
		ta_IN  = 0x0449, // Tamil
		tt_RU  = 0x0444, // Tatar
		te_IN  = 0x044a, // Telugu
		th_TH  = 0x041e, // Thai
		bo_BT  = 0x0851, // Tibetan - Bhutan
		bo_CN  = 0x0451, // Tibetan - PRC
		tr_TR  = 0x041f, // Turkish
		tk_TM  = 0x0442, // Turkmen - Cyrillic
		ug_CN  = 0x0480, // Uighur - Arabic
		uk_UA  = 0x0422, // Ukrainian
		wen_DE = 0x042e, // Upper Sorbian - Germany
		ur_PK  = 0x0420, // Urdu
		ur_IN  = 0x0820, // Urdu - India
		uz_UZ  = 0x0443, // Uzbek - Latin
		//uz_UZ  = 0x0843, // Uzbek - Cyrillic
		vi_VN  = 0x042a, // Vietnamese
		cy_GB  = 0x0452, // Welsh
		wo_SN  = 0x0488, // Wolof - Senegal
		xh_ZA  = 0x0434, // Xhosa - South Africa
		sah_RU = 0x0485, // Yakut - Cyrillic
		ii_CN  = 0x0478, // Yi - PRC
		yo_NG  = 0x046a, // Yoruba - Nigeria
		zu_ZA  = 0x0435, // Zulu
	};
}