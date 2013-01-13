#ifndef METAINFO_H
#define METAINFO_H

#include <QString>
#include <QStringList>
#include <QTime>

struct MetaInfoSingleFile
{
    qint64 length;
    QByteArray md5sum;
    QString name;
    int pieceLength;
    QList<QByteArray> sha1Sums;
};

struct MetaInfoMultiFile
{
    qint64 length;
    QByteArray md5sum;
    QString path;
};

class MetaInfo
{
public:
    MetaInfo();
    bool parse(const QByteArray &file);

    enum FileForm {
        SingleFileForm,
        MultiFileForm
    };

    QByteArray infoValue() const;

    FileForm fileForm() const;
    QString announceUrl() const;
    QStringList announceList() const;
    QDateTime creationDate() const;
    QString comment() const;
    QString createdBy() const;
    // For single file form
    MetaInfoSingleFile singleFile() const;

    // For multifile form
    QList<MetaInfoMultiFile> multiFiles() const;
    QString name() const;
    int pieceLength() const;
    QList<QByteArray> sha1Sums() const;

    // Total size
    qint64 totalSize() const;
private:
    void clear();

    QString mInfo;
    QByteArray mInfoSection;
    FileForm mFileForm;
    QString mAnnounceUrl;
    QStringList mAnnounceList;
    QString mComment;
    QString mCreatedBy;
    QDateTime mCreatedDate;
    QString mEncoding;
    QString mPublcisher;
    QString mPublcisherUrl;
    QString mName;
    int mPieceLength;
    QList<QByteArray> mSha1Sums;

    MetaInfoSingleFile mSingleFile;
    QList<MetaInfoMultiFile> mMultiFiles;
};

#endif // METAINFO_H
