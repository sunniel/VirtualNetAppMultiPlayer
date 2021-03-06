//
// Generated file, do not edit! Created by nedtool 5.4 from messages/InitApp.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __INITAPP_M_H
#define __INITAPP_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/InitApp.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet InitApp
 * {
 *     unsigned short peerId;
 * }
 * </pre>
 */
class InitApp : public ::omnetpp::cPacket
{
  protected:
    unsigned short peerId;

  private:
    void copy(const InitApp& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const InitApp&);

  public:
    InitApp(const char *name=nullptr, short kind=0);
    InitApp(const InitApp& other);
    virtual ~InitApp();
    InitApp& operator=(const InitApp& other);
    virtual InitApp *dup() const override {return new InitApp(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual unsigned short getPeerId() const;
    virtual void setPeerId(unsigned short peerId);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const InitApp& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, InitApp& obj) {obj.parsimUnpack(b);}


#endif // ifndef __INITAPP_M_H

