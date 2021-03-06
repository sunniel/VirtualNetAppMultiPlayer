//
// Generated file, do not edit! Created by nedtool 5.4 from messages/Timeout.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __TIMEOUT_M_H
#define __TIMEOUT_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



/**
 * Class generated from <tt>messages/Timeout.msg:19</tt> by nedtool.
 * <pre>
 * //
 * // TODO generated message class
 * //
 * packet Timeout
 * {
 *     string hostName;
 *     simtime_t timeout;
 * 
 *     // for client event timeout use
 *     int seq;
 *     string event;
 * }
 * </pre>
 */
class Timeout : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string hostName;
    ::omnetpp::simtime_t timeout;
    int seq;
    ::omnetpp::opp_string event;

  private:
    void copy(const Timeout& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const Timeout&);

  public:
    Timeout(const char *name=nullptr, short kind=0);
    Timeout(const Timeout& other);
    virtual ~Timeout();
    Timeout& operator=(const Timeout& other);
    virtual Timeout *dup() const override {return new Timeout(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getHostName() const;
    virtual void setHostName(const char * hostName);
    virtual ::omnetpp::simtime_t getTimeout() const;
    virtual void setTimeout(::omnetpp::simtime_t timeout);
    virtual int getSeq() const;
    virtual void setSeq(int seq);
    virtual const char * getEvent() const;
    virtual void setEvent(const char * event);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const Timeout& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, Timeout& obj) {obj.parsimUnpack(b);}


#endif // ifndef __TIMEOUT_M_H

