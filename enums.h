#ifndef ENUMS_H
#define ENUMS_H

#include <QObject>

const QString caseCatArr[] = {QObject::tr("гражданское"),             QObject::tr("уголовное"),
                              QObject::tr("административное"),        QObject::tr("арбитраж"),
                              QObject::tr("кодекс админ. судопр-ва"), QObject::tr("иное")};

const QString eventCatArr[] = {QObject::tr("судебное"),        QObject::tr("встреча"),
                              QObject::tr("подача документа"), QObject::tr("обжалование"),
                              QObject::tr("иное")};

enum class FIZ: unsigned char
{
    id,
    SURNAME,
    NAME,
    MIDDLE,
    BIRTHDAY,
    TELM,
    EMAIL,
    PASSER,
    PASNUM,
    WHENIS,
    ISBYWHOM,
    PASCODE,
    TELW,
    IND,
    CITY,
    STREET,
    BLD,
    CORP,
    FLAT,
    NOTES
};

enum class YUR: unsigned char
{
    id,
    OPF,
    COMPANY,
    OGRN,
    INN,
    TEL,
    EMAIL,
    DELEGATE,
    SURDEL,
    NAMEDEL,
    MIDDEL,
    IND,
    CITY,
    STREET,
    BLD,
    CORP,
    ROOM,
    DOC,
    REQS,
    BSURNAME,
    BNAME,
    BMIDDLE,
    NOTES
};

enum class Category: unsigned char
{
    FIZ,
    YUR
};

enum class CASE: unsigned char
{
    ID_CASE,
    ID_CLIENT,
    CLIENTSTATUS,
    CASECAT,
    CASEDATE,
    CONTRACTNUM,
    CONTRACTSUM,
    MAINPOINT,
    CASEPARTS,
    RULESOFLAW,
    PAYDATE,
    NOTES
};

enum class CaseCategory: unsigned char
{
    CIVIL,  //гражданское
    CRIME,  //уголовное
    ADMIN,  //административное
    ARBITR, //арбитраж
    CAS,    //кодекс административного судопроизводства
    OTHER   //иное
};

enum class ClientStatus: unsigned char
{
    APPLICANT, //истец
    RESPONDER, //ответчик
    THIRD,     //третье лицо
    ACCUSED,   //обвиняемый
    VICTIM,    //потерпевший
    ATTRACTED, //привлекаемый к админ. ответственности
    ADMINAPP,  //админ. истец
    OTHER      //иное
};

enum class File: unsigned char
{
    ID_DOC,
    CAT_DOC,
    CAT_OWNER,
    FNAME,
    FDATE,
    FSIZE
};

enum class DocViews: unsigned char
{
    FOTO,
    AUDIO,
    TEXT
};

enum class OwnerCat: unsigned char
{
    USER,
    CLIENT,
    CASE
};

enum class Events: unsigned char
{
    ID_EVE,
    DATE_E,
    CAT_EVE,
    ENAME,
    EDET,
    DATE_CR,
    CAT_OWNER,
    NUMD
};

#endif // ENUMS_H
