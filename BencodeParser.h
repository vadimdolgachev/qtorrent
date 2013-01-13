#ifndef BENCODEPARSER_H
#define BENCODEPARSER_H

#include <QString>
#include <QVariant>

typedef QMap<QByteArray,QVariant> Dictionary;
Q_DECLARE_METATYPE(Dictionary)

class BencodeParser
{
public:
    BencodeParser();
    QByteArray infoSection() const;
    Dictionary dictionary() const;
    bool parse(const QByteArray &file);

private:
    bool getDictonary(QMap<QByteArray, QVariant> *dic);
    bool getList(QList<QVariant> *list);
    bool getString(QByteArray *string);
    bool getInteger(QByteArray *integer);
    bool getValue(QVariant *value);

    int pos;
    int mBegInfo;
    int mEndInfo;
    QByteArray data;
    Dictionary dictionaryValue;
};

#endif // BENCODEPARSER_H
