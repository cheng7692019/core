/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of the LibreOffice project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * This file incorporates work covered by the following license notice:
 *
 *   Licensed to the Apache Software Foundation (ASF) under one or more
 *   contributor license agreements. See the NOTICE file distributed
 *   with this work for additional information regarding copyright
 *   ownership. The ASF licenses this file to you under the Apache
 *   License, Version 2.0 (the "License"); you may not use this file
 *   except in compliance with the License. You may obtain a copy of
 *   the License at http://www.apache.org/licenses/LICENSE-2.0 .
 */


#include "XMLIndexAlphabeticalSourceContext.hxx"

#include <com/sun/star/beans/XPropertySet.hpp>
#include <com/sun/star/container/XNameContainer.hpp>

#include <sax/tools/converter.hxx>

#include "XMLIndexTemplateContext.hxx"
#include <xmloff/xmlictxt.hxx>
#include <xmloff/xmlimp.hxx>
#include <xmloff/txtimp.hxx>
#include <xmloff/xmlnamespace.hxx>
#include <xmloff/xmltoken.hxx>
#include <xmloff/xmluconv.hxx>
#include <rtl/ustring.hxx>


using ::com::sun::star::beans::XPropertySet;
using ::com::sun::star::uno::Reference;
using ::com::sun::star::uno::Any;
using ::com::sun::star::xml::sax::XAttributeList;
using ::xmloff::token::IsXMLToken;
using namespace ::xmloff::token;

XMLIndexAlphabeticalSourceContext::XMLIndexAlphabeticalSourceContext(
    SvXMLImport& rImport,
    Reference<XPropertySet> & rPropSet)
:   XMLIndexSourceBaseContext(rImport, rPropSet, false)
,   bMainEntryStyleNameOK(false)
,   bSeparators(false)
,   bCombineEntries(true)
,   bCaseSensitive(true)
,   bEntry(false)
,   bUpperCase(false)
,   bCombineDash(false)
,   bCombinePP(true)
,   bCommaSeparated(false)
{
}

XMLIndexAlphabeticalSourceContext::~XMLIndexAlphabeticalSourceContext()
{
}

void XMLIndexAlphabeticalSourceContext::ProcessAttribute(
    sal_Int32 nAttributeToken,
    const OUString& rValue)
{
    bool bTmp(false);

    switch (nAttributeToken)
    {
        case XML_ELEMENT(TEXT, XML_MAIN_ENTRY_STYLE_NAME):
            {
                sMainEntryStyleName = rValue;
                OUString sDisplayStyleName = GetImport().GetStyleDisplayName(
                    XmlStyleFamily::TEXT_TEXT, sMainEntryStyleName );
                const Reference < css::container::XNameContainer >&
                    rStyles = GetImport().GetTextImport()->GetTextStyles();
                bMainEntryStyleNameOK = rStyles.is() && rStyles->hasByName( sDisplayStyleName );
            }
            break;

        case XML_ELEMENT(TEXT, XML_IGNORE_CASE):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCaseSensitive = !bTmp;
            }
            break;

        case XML_ELEMENT(TEXT, XML_ALPHABETICAL_SEPARATORS):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bSeparators = bTmp;
            }
            break;

        case XML_ELEMENT(TEXT, XML_COMBINE_ENTRIES):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCombineEntries = bTmp;
            }
            break;

        case XML_ELEMENT(TEXT, XML_COMBINE_ENTRIES_WITH_DASH):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCombineDash = bTmp;
            }
            break;
        case XML_ELEMENT(TEXT, XML_USE_KEYS_AS_ENTRIES):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bEntry = bTmp;
            }
            break;

        case XML_ELEMENT(TEXT, XML_COMBINE_ENTRIES_WITH_PP):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCombinePP = bTmp;
            }
            break;

        case XML_ELEMENT(TEXT, XML_CAPITALIZE_ENTRIES):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bUpperCase = bTmp;
            }
            break;

        case XML_ELEMENT(TEXT, XML_COMMA_SEPARATED):
            if (::sax::Converter::convertBool(bTmp, rValue))
            {
                bCommaSeparated = bTmp;
            }
            break;

        case XML_ELEMENT(TEXT, XML_SORT_ALGORITHM):
            sAlgorithm = rValue;
            break;
        case XML_ELEMENT(STYLE, XML_RFC_LANGUAGE_TAG):
            maLanguageTagODF.maRfcLanguageTag = rValue;
            break;
        case XML_ELEMENT(FO, XML_LANGUAGE):
            maLanguageTagODF.maLanguage = rValue;
            break;
        case XML_ELEMENT(FO, XML_SCRIPT):
            maLanguageTagODF.maScript = rValue;
            break;
        case XML_ELEMENT(FO, XML_COUNTRY):
            maLanguageTagODF.maCountry = rValue;
            break;

        default:
            XMLIndexSourceBaseContext::ProcessAttribute(nAttributeToken, rValue);
            break;
    }
}

void XMLIndexAlphabeticalSourceContext::endFastElement(sal_Int32 nElement)
{

    Any aAny;

    if (bMainEntryStyleNameOK)
    {
        aAny <<= GetImport().GetStyleDisplayName(
                            XmlStyleFamily::TEXT_TEXT, sMainEntryStyleName );
        rIndexPropertySet->setPropertyValue("MainEntryCharacterStyleName",aAny);
    }

    rIndexPropertySet->setPropertyValue("UseAlphabeticalSeparators", css::uno::Any(bSeparators));
    rIndexPropertySet->setPropertyValue("UseCombinedEntries", css::uno::Any(bCombineEntries));
    rIndexPropertySet->setPropertyValue("IsCaseSensitive", css::uno::Any(bCaseSensitive));
    rIndexPropertySet->setPropertyValue("UseKeyAsEntry", css::uno::Any(bEntry));
    rIndexPropertySet->setPropertyValue("UseUpperCase", css::uno::Any(bUpperCase));
    rIndexPropertySet->setPropertyValue("UseDash", css::uno::Any(bCombineDash));
    rIndexPropertySet->setPropertyValue("UsePP", css::uno::Any(bCombinePP));
    rIndexPropertySet->setPropertyValue("IsCommaSeparated", css::uno::Any(bCommaSeparated));


    if (!sAlgorithm.isEmpty())
    {
        rIndexPropertySet->setPropertyValue("SortAlgorithm", css::uno::Any(sAlgorithm));
    }

    if ( !maLanguageTagODF.isEmpty() )
    {
        aAny <<= maLanguageTagODF.getLanguageTag().getLocale( false);
        rIndexPropertySet->setPropertyValue("Locale", aAny);
    }

    XMLIndexSourceBaseContext::endFastElement(nElement);
}

css::uno::Reference< css::xml::sax::XFastContextHandler > XMLIndexAlphabeticalSourceContext::createFastChildContext(
    sal_Int32 nElement,
    const css::uno::Reference< css::xml::sax::XFastAttributeList >& xAttrList )
{
    if ( nElement == XML_ELEMENT(TEXT, XML_ALPHABETICAL_INDEX_ENTRY_TEMPLATE) )
    {
        return new XMLIndexTemplateContext(GetImport(), rIndexPropertySet,
                                           aLevelNameAlphaMap,
                                           XML_OUTLINE_LEVEL,
                                           aLevelStylePropNameAlphaMap,
                                           aAllowedTokenTypesAlpha);
    }
    else
    {
        return XMLIndexSourceBaseContext::createFastChildContext(nElement,
                                                             xAttrList);
    }
}

/* vim:set shiftwidth=4 softtabstop=4 expandtab: */
