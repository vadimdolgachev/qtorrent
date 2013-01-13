#include "MetaInfo.h"
#include "BencodeParser.h"

#include <QDebug>

MetaInfo::MetaInfo()
{
}

bool MetaInfo::parse(const QByteArray &file)
{
    clear();
    BencodeParser bendecode;
    if (bendecode.parse(file)) {
        const Dictionary &dic = bendecode.dictionary();
        mInfo = dic.value("announce").toString();
        mInfoSection = bendecode.infoSection();

        QList<QVariant> announce_list =  dic.value("announce-list").value<QList<QVariant> >();
        foreach (QVariant subList, announce_list) {
            QList<QVariant> list = subList.value<QList<QVariant> > ();
            foreach (QVariant url, list) {
                mAnnounceList.append(url.toString());
            }
        }
        mComment = dic.value("comment").toString();
        mCreatedBy = dic.value("created by").toString();
        mCreatedDate.setTime_t(dic.value("creation date").toInt());
        mPublcisher = dic.value("publisher").toString();
        mPublcisherUrl = dic.value("publisher-url").toString();
        mAnnounceUrl = dic.value("announce").toString();

        QMap<QByteArray, QVariant> info = dic.value("info").value<Dictionary>();

        mPieceLength = info.value("piece length").toUInt();

        if (info.contains("files")) {
            mFileForm = MultiFileForm;

            QList<QVariant> files = info.value("files").toList();

            for (int i = 0; i < files.size(); ++i) {
                QMap<QByteArray, QVariant> file = files.at(i).value<Dictionary>();
                QList<QVariant> pathElements = file.value("path").toList();
                QByteArray path;
                foreach (QVariant p, pathElements) {
                    if (!path.isEmpty())
                        path += "/";
                    path += p.toByteArray();
                }

                MetaInfoMultiFile multiFile;
                multiFile.length = file.value("length").toLongLong();
                multiFile.path = QString::fromUtf8(path);
                multiFile.md5sum = file.value("md5sum").toByteArray();
                mMultiFiles.append(multiFile);
            }

        } else if (info.contains("length")) {
            mFileForm = SingleFileForm;
            mSingleFile.length = info.value("length").toULongLong();
            mSingleFile.name = info.value("name").toString();
            mSingleFile.pieceLength = info.value("piece length").toUInt();
            mSingleFile.md5sum = info.value("md5sum").toByteArray();
            QByteArray pieces = info.value("pieces").toByteArray();
            for (int i = 0; i < pieces.size(); i += 20)
                mSingleFile.sha1Sums << pieces.mid(i, 20);
        }
        return true;
    }
    return false;
}

QByteArray MetaInfo::infoValue() const
{
    return mInfoSection;
}

MetaInfo::FileForm MetaInfo::fileForm() const
{
    return mFileForm;
}

QString MetaInfo::announceUrl() const
{
    return mAnnounceUrl;
}

QStringList MetaInfo::announceList() const
{
    return mAnnounceList;
}

QDateTime MetaInfo::creationDate() const
{
    return mCreatedDate;
}

QString MetaInfo::comment() const
{
    return mComment;
}

QString MetaInfo::createdBy() const
{
    return mCreatedBy;
}

MetaInfoSingleFile MetaInfo::singleFile() const
{
    return mSingleFile;
}

QList<MetaInfoMultiFile> MetaInfo::multiFiles() const
{
    return mMultiFiles;
}

QString MetaInfo::name() const
{
    return mName;
}

int MetaInfo::pieceLength() const
{
    return mPieceLength;
}

QList<QByteArray> MetaInfo::sha1Sums() const
{
    return mSha1Sums;
}

qint64 MetaInfo::totalSize() const
{
    if (mFileForm == SingleFileForm)
        return mSingleFile.length;

    qint64 length = 0;
    foreach (const MetaInfoMultiFile &file, mMultiFiles)
        length += file.length;
    return length;
}

void MetaInfo::clear()
{
    mMultiFiles.clear();
    mSha1Sums.clear();
}
