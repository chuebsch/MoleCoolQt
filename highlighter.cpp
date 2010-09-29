 /****************************************************************************
 **
 ** Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
 **
 ** This file is part of the example classes of the Qt Toolkit.
 **
 ** This file may be used under the terms of the GNU General Public
 ** License version 2.0 as published by the Free Software Foundation
 ** and appearing in the file LICENSE.GPL included in the packaging of
 ** this file.  Please review the following information to ensure GNU
 ** General Public Licensing requirements will be met:
 ** http://trolltech.com/products/qt/licenses/licensing/opensource/
 **
 ** If you are unsure which license is appropriate for your use, please
 ** review the following information:
 ** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
 ** or contact the sales department at sales@trolltech.com.
 **
 ** In addition, as a special exception, Trolltech gives you certain
 ** additional rights. These rights are described in the Trolltech GPL
 ** Exception version 1.0, which can be found at
 ** http://www.trolltech.com/products/qt/gplexception/ and in the file
 ** GPL_EXCEPTION.txt in this package.
 **
 ** In addition, as a special exception, Trolltech, as the sole copyright
 ** holder for Qt Designer, grants users of the Qt/Eclipse Integration
 ** plug-in the right for the Qt/Eclipse Integration to link to
 ** functionality provided by Qt Designer and its related libraries.
 **
 ** Trolltech reserves all rights not expressly granted herein.
 **
 ** Trolltech ASA (c) 2007
 **
 ** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 ** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 **
 ****************************************************************************/

#include <QtGui>

#include "highlighter.h"

 Highlighter::Highlighter(QTextDocument *parent)
   : QSyntaxHighlighter(parent)
{
  HighlightingRule rule;
  
  keywordFormat.setForeground(Qt::darkRed);
  keywordFormat.setFontWeight(QFont::Bold);
  QStringList keywordPatterns;
  keywordPatterns << "\\bTITLE\\b" << "\\bEND\\b" << "^KEY     XYZ --U2-- ----U3---- ------U4------- M- -D- --Q-- ---O--- ----H----"
		  << "\\bCELL\\b" << "\\bWAVE\\b" << "\\bLATT\\b"
		  << "\\bSYMM\\b" << "\\bBANK\\b" << "\\bMODULE\\b"
		  << "\\bSELECT\\b" << "\\bSAVE\\b" << "\\bSCAT\\b"
		  << "\\bSOLVE\\b" << "\\bKEEP\\b" << "\\bWEIGHT\\b"
		  << "\\bSKIP\\b" << "\\bPRINT\\b" << "\\bDMSDA\\b"
		  << "\\bFOUR\\b" << "\\bGRID\\b" << "\\bLIMITS\\b"<< "\\bCELLSD\\b"
		  << "\\bMODEL\\b" << "\\bPROPERTY\\b"<< "^ATOM     ATOM0    AX1 ATOM1    ATOM2   AX2 R/L TP TBL KAP LMX SITESYM  CHEMCON" ;
  foreach (QString pattern, keywordPatterns) {
    rule.pattern = QRegExp(pattern);
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  keybitsFormat.setFontWeight(QFont::Black);
  keybitsFormat.setForeground(Qt::white);
  keybitsFormat.setBackground(Qt::darkGray);
  rule.pattern =  QRegExp("\\b[01]{3,3} [01]{6,6} [01]{10,10} [01]{15,15} [01]{2,2} [01]{3,3} [01]{5,5} [01]{7,7} [01]{9,9}");
  rule.format = keybitsFormat;
  highlightingRules.append(rule);
  
  classFormat.setFontWeight(QFont::Bold);
  classFormat.setForeground(Qt::darkMagenta);
  rule.pattern = QRegExp("\\bDUM[0-9]+\\b");
  rule.format = classFormat;
  highlightingRules.append(rule);

  QTextCharFormat xyzFormat;
  xyzFormat.setFontWeight(QFont::Bold);
  xyzFormat.setForeground(QColor(128,128,255));
  xyzFormat.setBackground(Qt::darkBlue);
  rule.pattern = QRegExp("\\bX\\b");
  rule.format = xyzFormat;
  highlightingRules.append(rule);
  xyzFormat.setForeground(QColor(255,128,128));
  xyzFormat.setBackground(Qt::darkRed);
  rule.pattern = QRegExp("\\bY\\b");
  rule.format = xyzFormat;
  highlightingRules.append(rule);
  xyzFormat.setForeground(QColor(128,255,128));
  xyzFormat.setBackground(Qt::darkGreen);
  rule.pattern = QRegExp("\\bZ\\b");
  rule.format = xyzFormat;
  highlightingRules.append(rule);

  functionFormat.setFontItalic(true);
  functionFormat.setForeground(Qt::red);
  rule.pattern = QRegExp("\\b[A-Za-z0-9_+-]+\\(\\w+\\)");
  rule.format = functionFormat;
  highlightingRules.append(rule);

  numberFormat.setFontWeight(QFont::Bold);
  numberFormat.setForeground(Qt::darkCyan);
  rule.pattern =  QRegExp("\\b[0-9]+\\.[0-9]+\\b");
  rule.format = numberFormat;
  highlightingRules.append(rule);

  staredFormat.setFontUnderline (true);
  staredFormat.setFontWeight(QFont::Black);
  staredFormat.setForeground(Qt::black);
  staredFormat.setBackground(Qt::green);
  rule.pattern =  QRegExp("\\*\\S+");
  rule.format = staredFormat;
  highlightingRules.append(rule);

  singleLineCommentFormat.setForeground(Qt::blue);
  rule.pattern = QRegExp("![^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);
  
  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("\".*\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);
  
  toolongFormat.setFontWeight(QFont::Black);
  toolongFormat.setForeground(Qt::white);
  toolongFormat.setBackground(Qt::red);
  rule.pattern =  QRegExp("^[^\n]{118,}");
  rule.format = toolongFormat;
  highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
  foreach (HighlightingRule rule, highlightingRules) {
    QRegExp expression(rule.pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = text.indexOf(expression, index + length);
    }
  }
  setCurrentBlockState(0);
  /*
  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = text.indexOf(commentStartExpression);
  
  while (startIndex >= 0) {
    int endIndex = text.indexOf(commentEndExpression, startIndex);
    int commentLength;
    if (endIndex == -1) {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    } else {
      commentLength = endIndex - startIndex
                             + commentEndExpression.matchedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex = text.indexOf(commentStartExpression,
			      startIndex + commentLength);
  }
  */
}
