#pragma once

#include <QObject>
#include <QVariant>
#include <QVariantList>
#include <QMetaObject>
#include <QString>
#include <QMetaProperty>


// t: type, x: property name
#define PropertyPrivateSet(t, x) private: t _##x;  \
    public: t x() const {return _##x;} \
    private: void x(const t& v){if (_##x != v) {_##x = v; emit x##Changed(v);}}

// t: type, x: property name
#define PropertyProtectedSet(t, x) private: t _##x;  \
    public: t x() const {return _##x;} \
    protected: void x(const t& v){if (_##x != v) {_##x = v; emit x##Changed(v);}}

// t: type, x: property name
#define PropertyPublicSet(t, x) private: t _##x;  \
    public: t x() const {return _##x;} \
    void x(const t& v){if (_##x != v) {_##x = v; emit x##Changed(v);}}


// t: type, x: property name
#define MetaPropertyPrivateSet_List(t, x)  Q_SIGNALS: void x##Changed(const QVariantList& i); \
    private: Q_PROPERTY(QVariantList x READ x WRITE x NOTIFY x##Changed) \
    PropertyPrivateSet(QVariantList, x) \
    void append##x(const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.append(v); emit x##Changed(_##x);}} \
    void insert##t(int ind, const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.insert(ind,v); emit x##Changed(_##x);}} \
    void remove##x(const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);}} \
    public: \
    Q_INVOKABLE int count##x()const {return _##x.length();} \
    Q_INVOKABLE t item##x##At(int i) {return _##x.at(i).value<t>();} \
    Q_INVOKABLE bool item##x##Exist(const t& i) const {QVariant v = QVariant::fromValue(i); if (v.isValid()) { return _##x.contains(v);} else return false;}


// t: type, x: property name
#define MetaPropertyProtectedSet_List(t, x)  Q_SIGNALS: void x##Changed(const QVariantList& i); \
    private: Q_PROPERTY(QVariantList x READ x WRITE x NOTIFY x##Changed) \
    PropertyProtectedSet(QVariantList, x) \
    void append##x(const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.append(v); emit x##Changed(_##x);}} \
    void insert##t(int ind, const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.insert(ind,v); emit x##Changed(_##x);}} \
    void remove##x(const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);}} \
    public: \
    Q_INVOKABLE int count##x()const {return _##x.length();} \
    Q_INVOKABLE t item##x##At(int i) {return _##x.at(i).value<t>();} \
    Q_INVOKABLE bool item##x##Exist(const t& i) const {QVariant v = QVariant::fromValue(i); if (v.isValid()) { return _##x.contains(v);} else return false;}


// t: type, x: property name
#define MetaPropertyPublicSet_List(t, x)  Q_SIGNALS: void x##Changed(const QVariantList& i); \
    private: Q_PROPERTY(QVariantList x READ x WRITE x NOTIFY x##Changed) \
    PropertyPublicSet(QVariantList, x) \
    public: \
    Q_INVOKABLE void append##x(const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.append(v); emit x##Changed(_##x);}} \
    Q_INVOKABLE void insert##t(int ind, const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.insert(ind,v); emit x##Changed(_##x);}} \
    Q_INVOKABLE void remove##x(const t& i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);}} \
    Q_INVOKABLE void clear##x() {_##x.clear();  emit x##Changed(_##x);} \
    Q_INVOKABLE int count##x()const {return _##x.length();} \
    Q_INVOKABLE t item##x##At(int i) {return _##x.at(i).value<t>();} \
    Q_INVOKABLE bool item##x##Exist(const t& i) const {QVariant v = QVariant::fromValue(i); if (v.isValid()) { return _##x.contains(v);} else return false;}

// t: type, x: property name
#define PropertyPrivateSet_Ptr(t, x) private: t* _##x;  \
    public: t* x() {return _##x;} \
    private: void x(t* v){_##x = v;}


// t: type, x: property name
#define PropertyProtectedSet_Ptr(t, x) private: t* _##x;  \
    public: t* x() {return _##x;} \
    protected: void x(t* v){_##x = v;}

// t: type, x: property name
#define PropertyPublicSet_Ptr(t, x) private: t* _##x;  \
    public: t* x() {return _##x;} \
    void x(t* v){_##x = v;}


// t: type, x: property name
#define PropertyPrivateSet_Ptr_O(t, x) private: QObject* _##x;  \
    public: t* x() {return qobject_cast<t*>(_##x);} \
    private: void x(QObject* v){_##x = v;}


// t: type, x: property name
#define PropertyProtectedSet_Ptr_O(t, x) private: QObject* _##x;  \
    public: t* x() {return qobject_cast<t*>(_##x);} \
    protected: void x(QObject* v){_##x = v;}

// t: type, x: property name
#define PropertyPublicSet_Ptr_O(t, x)  private: QObject* _##x;  \
    public: t* x() {return qobject_cast<t*>(_##x);} \
    public: void x(QObject* v){_##x = v;}

// t: type, x: property name
#define MetaPropertyPrivateSet(t, x) Q_SIGNALS: void x##Changed(const t& i); \
    private: Q_PROPERTY(t x READ x WRITE x NOTIFY x##Changed) \
    PropertyPrivateSet(t, x)


// t: type, x: property name
#define MetaPropertyProtectedSet(t, x) Q_SIGNALS: void x##Changed(const t& i); \
    private: Q_PROPERTY(t x READ x WRITE x NOTIFY x##Changed) \
    PropertyProtectedSet(t, x)

// t: type, x: property name
#define MetaPropertyPublicSet(t, x) Q_SIGNALS: void x##Changed(const t& i); \
    private: Q_PROPERTY(t x READ x WRITE x NOTIFY x##Changed) \
    PropertyPublicSet(t, x)

#define MetaPropertyPublicSetWithModifyeGuard(t, x) \
    MetaPropertyPublicSet(t, x) \
    MetaPropertyPublicSet(bool, x##ModifyGuard) \

// t: type, x: property name
#define MetaPropertyPrivateSet_Ptr(t, x) Q_SIGNALS: void x##Changed(QObject* i); \
    private: Q_PROPERTY(QObject* x READ x WRITE x NOTIFY x##Changed) \
    PropertyPrivateSet_Ptr_O(t, x) \
    public: Q_INVOKABLE void _type##x(const QString& prop){registerPtrProperty(prop, &t::staticMetaObject);}


// t: type, x: property name
#define MetaPropertyProtectedSet_Ptr(t, x) Q_SIGNALS: void x##Changed(QObject* i); \
    private: Q_PROPERTY(QObject* x READ x WRITE x NOTIFY x##Changed) \
    PropertyProtectedSet_Ptr_O(t, x) \
    public: Q_INVOKABLE void _type##x(const QString& prop){registerPtrProperty(prop, &t::staticMetaObject);}


// t: type, x: property name
#define MetaPropertyPublicSet_Ptr(t, x) Q_SIGNALS: void x##Changed(QObject* i); \
    private: Q_PROPERTY(QObject* x READ x WRITE x NOTIFY x##Changed) \
    PropertyPublicSet_Ptr_O(t, x) \
    public: Q_INVOKABLE void _type##x(const QString& prop){registerPtrProperty(prop, &t::staticMetaObject);}

// t: type, x: property name
#define MetaPropertyPrivateSet_Ptr_List(t, x)  Q_SIGNALS: void x##Changed(const QVariantList& i); \
    private: Q_PROPERTY(QVariantList x READ x WRITE x NOTIFY x##Changed) \
    PropertyPrivateSet(QVariantList, x) \
    private: Q_INVOKABLE void _type##x(const QString& prop){registerPtrProperty(prop, &t::staticMetaObject);} \
    void append##t(t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.append(v); emit x##Changed(_##x);}} \
    void insert##t(int ind, t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.insert(ind,v); emit x##Changed(_##x);}} \
    void remove##t(t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);}} \
    t* remove##t##At(int ind){t* i = _##x.at(ind).value<t*>(); QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);} return i;} \
    Q_INVOKABLE void removeAll##t() {for (int i = _##x.length() - 1; i >= 0; i--) (_##x.at(i).value<t*>())->deleteLater(); _##x.clear(); emit x##Changed(_##x);} \
    public: Q_INVOKABLE int count##t()const {return _##x.length();} \
    Q_INVOKABLE t* item##t##At(int i) {return _##x.at(i).value<t*>();} \
    Q_INVOKABLE bool item##t##Exist(t* i) const {QVariant v = QVariant::fromValue(i); if (v.isValid()) { return _##x.contains(v);} else return false;}


// t: type, x: property name
#define MetaPropertyProtectedSet_Ptr_List(t, x)  Q_SIGNALS: void x##Changed(const QVariantList& i); \
    private: Q_PROPERTY(QVariantList x READ x WRITE x NOTIFY x##Changed) \
    PropertyProtectedSet(QVariantList, x) \
    protected: Q_INVOKABLE void _type##x(const QString& prop){registerPtrProperty(prop, &t::staticMetaObject);} \
    void append##t(t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.append(v); emit x##Changed(_##x);}} \
    void insert##t(int ind, t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.insert(ind,v); emit x##Changed(_##x);}} \
    void remove##t(t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);}} \
    t* remove##t##At(int ind){t* i = _##x.at(ind).value<t*>(); QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);} return i;} \
    Q_INVOKABLE void removeAll##t() {for (int i = _##x.length() - 1; i >= 0; i--) (_##x.at(i).value<t*>())->deleteLater(); _##x.clear(); emit x##Changed(_##x);} \
    public: Q_INVOKABLE int count##t()const {return _##x.length();} \
    Q_INVOKABLE t* item##t##At(int i) {return _##x.at(i).value<t*>();} \
    Q_INVOKABLE bool item##t##Exist(t* i) const {QVariant v = QVariant::fromValue(i); if (v.isValid()) { return _##x.contains(v);} else return false;}


// t: type, x: property name
#define MetaPropertyPublicSet_Ptr_List(t, x)  Q_SIGNALS: void x##Changed(const QVariantList& i); \
    private: Q_PROPERTY(QVariantList x READ x WRITE x NOTIFY x##Changed) \
    PropertyPublicSet(QVariantList, x) \
    public: Q_INVOKABLE void _type##x(const QString& prop){registerPtrProperty(prop, &t::staticMetaObject);} \
    Q_INVOKABLE void append##t(t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.append(v);  emit x##Changed(_##x);}} \
    Q_INVOKABLE void insert##t(int ind, t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.insert(ind,v); emit x##Changed(_##x);}} \
    Q_INVOKABLE void remove##t(t* i){QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);}} \
    Q_INVOKABLE t* remove##t##At(int ind){t* i = _##x.at(ind).value<t*>(); QVariant v = QVariant::fromValue(i); if (v.isValid()) { _##x.removeAll(v); emit x##Changed(_##x);} return i;} \
    Q_INVOKABLE int count##t()const {return _##x.length();} \
    Q_INVOKABLE void removeAll##t() {for (int i = _##x.length() - 1; i >= 0; i--) (_##x.at(i).value<t*>())->deleteLater(); _##x.clear(); emit x##Changed(_##x);} \
    Q_INVOKABLE t* item##t##At(int i) {return _##x.at(i).value<t*>();} \
    Q_INVOKABLE bool item##t##Exist(t* i) const {QVariant v = QVariant::fromValue(i); if (v.isValid()) { return _##x.contains(v);} else return false;}



// This *_Ex are for use in classes that does not inherit from JObject
// t: type, x: property name
#define MetaPropertyPrivateSet_Ex(t, x) Q_SIGNALS: void x##Changed(t i); \
    private: Q_PROPERTY(t x READ x WRITE x NOTIFY x##Changed) \
    private: t _##x;  \
    public: t x() {return _##x;} \
    private: void x(t v){if (_##x != v) {_##x = v; emit x##Changed(v);}}

// t: type, x: property name
#define MetaPropertyProtectedSet_Ex(t, x) Q_SIGNALS: void x##Changed(t i); \
    private: Q_PROPERTY(t x READ x WRITE x NOTIFY x##Changed) \
    private: t _##x;  \
    public: t x() {return _##x;} \
    protected: void x(t v){if (_##x != v) {_##x = v; emit x##Changed(v);}}

// t: type, x: property name
#define MetaPropertyPublicSet_Ex(t, x) Q_SIGNALS: void x##Changed(t i); \
    private: Q_PROPERTY(t x READ x WRITE x NOTIFY x##Changed) \
    private: t _##x;  \
    public: t x() {return _##x;} \
    public: void x(t v){if (_##x != v) {_##x = v; emit x##Changed(v);}}


/**
 *  Base class of classes that should import/export all its properties from/to QVariant via JSON.
 */
class JObject : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE explicit JObject(QObject *parent = 0);
    virtual void initAfterConstruct() {}
    virtual void initAfterImport() {}

    virtual QVariant exportToVariant();
    virtual bool importFromVariant(const QVariant& v);

    QByteArray exportToJson(bool indented = true);
    bool importFromJson(const QByteArray& json);

protected:
    void registerPtrProperty(const QString& prop, const QMetaObject* mo);
    const QMetaObject* metaObjectForProp(const QString& prop);

private:
    Q_DISABLE_COPY(JObject)
    QMap <QString, const QMetaObject* > _ptrPropMap;
};

