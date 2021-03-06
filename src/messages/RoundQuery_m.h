//
// Generated file, do not edit! Created by nedtool 5.4 from messages/RoundQuery.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __ROUNDQUERY_M_H
#define __ROUNDQUERY_M_H

#include <omnetpp.h>

// nedtool version check
#define MSGC_VERSION 0x0504
#if (MSGC_VERSION!=OMNETPP_VERSION)
#    error Version mismatch! Probably this file was generated by an earlier version of nedtool: 'make clean' should help.
#endif



// cplusplus {{
#include "../common/Constants.h"
// }}

/**
 * Class generated from <tt>messages/RoundQuery.msg:24</tt> by nedtool.
 * <pre>
 * packet RoundQuery
 * {
 *     string displayString = DISPLAY_STR_MSG_GROUP_EH;
 * 
 *     string senderName;
 *     string destName;
 *     int round;
 *     string seqRanges;
 * }
 * </pre>
 */
class RoundQuery : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string displayString;
    ::omnetpp::opp_string senderName;
    ::omnetpp::opp_string destName;
    int round;
    ::omnetpp::opp_string seqRanges;

  private:
    void copy(const RoundQuery& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const RoundQuery&);

  public:
    RoundQuery(const char *name=nullptr, short kind=0);
    RoundQuery(const RoundQuery& other);
    virtual ~RoundQuery();
    RoundQuery& operator=(const RoundQuery& other);
    virtual RoundQuery *dup() const override {return new RoundQuery(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getDisplayString() const;
    virtual void setDisplayString(const char * displayString);
    virtual const char * getSenderName() const;
    virtual void setSenderName(const char * senderName);
    virtual const char * getDestName() const;
    virtual void setDestName(const char * destName);
    virtual int getRound() const;
    virtual void setRound(int round);
    virtual const char * getSeqRanges() const;
    virtual void setSeqRanges(const char * seqRanges);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const RoundQuery& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, RoundQuery& obj) {obj.parsimUnpack(b);}


#endif // ifndef __ROUNDQUERY_M_H

