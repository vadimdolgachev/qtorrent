#include "BencodeParser.h"

#include <QString>
#include <QDebug>

BencodeParser::BencodeParser()
{
    pos = 0;
}

QByteArray BencodeParser::infoSection() const
{
    return data.mid(mBegInfo, mEndInfo - mBegInfo);
}

bool BencodeParser::parse(const QByteArray &file)
{
    data = file;
    return getDictonary(&dictionaryValue);
}

bool BencodeParser::getDictonary(QMap<QByteArray, QVariant> *dic)
{
    if (data.at(pos) == 'd') {
//        qDebug() << data.mid(pos, 1);
        ++pos;
        while (pos < data.size()) {
            //            qDebug() << "ok";
            QByteArray key;
            QVariant value;
//            qDebug() << "get key";
            if(!getString(&key))
                break;
            if (key == "info")
                mBegInfo = pos;
//            qDebug() << key;
            if(!getValue(&value))
                break;
            if (key == "info")
                mEndInfo = pos;
            dic->insert(key, value);
//            qDebug() << key << value;
            if (data.at(pos) == 'e')
                break;
        }
        ++pos;
        return true;
    }
    return false;
}

bool BencodeParser::getList(QList<QVariant> *list)
{
    if (data.at(pos) == 'l') {
        ++pos;
        while (pos < data.size()) {
            QVariant value;

            getValue(&value);
//            qDebug() << value;
            list->append(value);
            if (data.at(pos) == 'e')
                break;
        }
        ++pos;
        return true;
    }
    return false;
}

bool BencodeParser::getString(QByteArray *string)
{
    int tokenPos = data.indexOf(":", pos);
    if (tokenPos > 0) {
        bool ok;
        int size = data.mid(pos, tokenPos - pos).toInt(&ok);
//        qDebug() << data.mid(pos, tokenPos - pos);
        if (ok) {
            pos = tokenPos + 1;
            *string = data.mid(pos, size);
            pos += size;
            return true;
        } else {
            qDebug() << "not found size";
        }
    } else {
        qDebug() << "not found token :";
    }
    return false;
}

bool BencodeParser::getInteger(QByteArray *integer)
{
    if (data.at(pos) == 'i') {
        int end = data.indexOf('e', pos);
        if (end > 0) {
            ++pos;
            QByteArray str = data.mid(pos, end - pos);
            *integer = str;
            pos = end + 1;
            return true;
        } else {
            qDebug() << "wrong number";
        }
    }
    return false;
}

bool BencodeParser::getValue(QVariant *value)
{
    if (data.at(pos) == 'i') {
//        qDebug() << "int";
        QByteArray integer;
        bool ret = getInteger(&integer);
        *value = integer;
        return ret;
    } else if (data.at(pos) == 'd') {
//        qDebug() << "dictonary";
        QMap<QByteArray, QVariant> dic;
        bool ret = getDictonary(&dic);
        value->setValue(dic);
        return ret;
    } else if (data.at(pos) == 'l') {
//        qDebug() << "list";
        QList<QVariant> dic;
        bool ret = getList(&dic);
        value->setValue(dic);
        return ret;
    } else {
//        qDebug() << "string";
        QByteArray string;
        bool ret = getString(&string);
//        qDebug() << string;
        *value = string;
        return  ret;
    }
    return false;
}


Dictionary BencodeParser::dictionary() const
{
    return dictionaryValue;
}
