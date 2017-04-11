#include "LineAddedMsg.h"

int LineAddedMsg::TypeID =  -1;

LineAddedMsg::LineAddedMsg()
{
}

LineAddedMsg::LineAddedMsg(uint nDot1Row, uint nDot1Column, uint nDot2Row, uint nDot2Column):
        mnDot1Row(nDot1Row),mnDot1Column(nDot1Column),mnDot2Row(nDot2Row),mnDot2Column(nDot2Column)
{

}

LineAddedMsg::~LineAddedMsg()
{

}

void LineAddedMsg::setDot1Row(const uint nRow)
{
    mnDot1Row = nRow;
}

uint LineAddedMsg::dot1Row()const{return mnDot1Row;}

void LineAddedMsg::setDot1Column(const uint nColumn)
{
    mnDot1Column = nColumn;
}

uint LineAddedMsg::dot1Column()const{return mnDot1Column;}

void LineAddedMsg::setDot2Row(const uint nRow)
{
    mnDot2Row = nRow;
}

uint LineAddedMsg::dot2Row()const{return mnDot2Row;}

void LineAddedMsg::setDot2Column(const uint nColumn)
{
    mnDot2Column = nColumn;
}

uint LineAddedMsg::dot2Column()const{return mnDot2Column;}


void LineAddedMsg::write(QDataStream &buf)
{
    buf << typeId();
    buf << mnDot1Row;
    buf << mnDot1Column;
    buf << mnDot2Row;
    buf << mnDot2Column;
}


void LineAddedMsg::read(QDataStream &buf)
{
    buf >> mnDot1Row;
    buf >> mnDot1Column;
    buf >> mnDot2Row;
    buf >> mnDot2Column;
}

int LineAddedMsg::typeId()
{
    return LineAddedMsg::TypeID;
}
