//
// Generated file, do not edit! Created by nedtool 5.4 from messages/ConnectTimeout.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __CONNECTTIMEOUT_M_H
#define __CONNECTTIMEOUT_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/ConnectTimeout.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet ConnectTimeout
 * {
 *     string LCName;
 * }
 * </pre>
 */
class ConnectTimeout : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string LCName;

  private:
    void copy(const ConnectTimeout& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const ConnectTimeout&);

  public:
    ConnectTimeout(const char *name=nullptr, short kind=0);
    ConnectTimeout(const ConnectTimeout& other);
    virtual ~ConnectTimeout();
    ConnectTimeout& operator=(const ConnectTimeout& other);
    virtual ConnectTimeout *dup() const override {return new ConnectTimeout(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getLCName() const;
    virtual void setLCName(const char * LCName);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const ConnectTimeout& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, ConnectTimeout& obj) {obj.parsimUnpack(b);}


#endif // ifndef __CONNECTTIMEOUT_M_H

