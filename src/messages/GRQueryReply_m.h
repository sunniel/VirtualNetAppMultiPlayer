//
// Generated file, do not edit! Created by nedtool 5.4 from messages/GRQueryReply.msg.
//

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#ifndef __GRQUERYREPLY_M_H
#define __GRQUERYREPLY_M_H

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
 * Class generated from <tt>messages/GRQueryReply.msg:24</tt> by nedtool.
 * <pre>
 * packet GRQueryReply
 * {
 *     string displayString = DISPLAY_STR_MSG_GROUP_GR;
 * 
 *     string senderName;
 *     int epoch;
 *     int groupId;
 *     string state;
 * }
 * </pre>
 */
class GRQueryReply : public ::omnetpp::cPacket
{
  protected:
    ::omnetpp::opp_string displayString;
    ::omnetpp::opp_string senderName;
    int epoch;
    int groupId;
    ::omnetpp::opp_string state;

  private:
    void copy(const GRQueryReply& other);

  protected:
    // protected and unimplemented operator==(), to prevent accidental usage
    bool operator==(const GRQueryReply&);

  public:
    GRQueryReply(const char *name=nullptr, short kind=0);
    GRQueryReply(const GRQueryReply& other);
    virtual ~GRQueryReply();
    GRQueryReply& operator=(const GRQueryReply& other);
    virtual GRQueryReply *dup() const override {return new GRQueryReply(*this);}
    virtual void parsimPack(omnetpp::cCommBuffer *b) const override;
    virtual void parsimUnpack(omnetpp::cCommBuffer *b) override;

    // field getter/setter methods
    virtual const char * getDisplayString() const;
    virtual void setDisplayString(const char * displayString);
    virtual const char * getSenderName() const;
    virtual void setSenderName(const char * senderName);
    virtual int getEpoch() const;
    virtual void setEpoch(int epoch);
    virtual int getGroupId() const;
    virtual void setGroupId(int groupId);
    virtual const char * getState() const;
    virtual void setState(const char * state);
};

inline void doParsimPacking(omnetpp::cCommBuffer *b, const GRQueryReply& obj) {obj.parsimPack(b);}
inline void doParsimUnpacking(omnetpp::cCommBuffer *b, GRQueryReply& obj) {obj.parsimUnpack(b);}


#endif // ifndef __GRQUERYREPLY_M_H

