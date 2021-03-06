//
// Generated file, do not edit! Created by nedtool 5.4 from messages/Connect.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CONNECT_M_H
#define __CONNECT_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/Connect.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet Connect
 * {
 *     string LCName;
 *     unsigned short peerId;
 * }
 * </pre>
 */
class Connect : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string LCName;
    unsigned short peerId;

  private:
    void copy(const Connect& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Connect&);

  public:
    Connect(const char *name=nullptr, short kind=0);
    Connect(const Connect& other);
    virtual ~Connect();
    Connect& operator=(const Connect& other);
    virtual Connect *dup() const override {return new Connect(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getLCName() const;
    virtual void setLCName(const char * LCName);
    virtual unsigned short getPeerId() const;
    virtual void setPeerId(unsigned short peerId);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Connect& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Connect& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CONNECT_M_H

