/*
 * DaaS-IoT 2019, 2025 (@) Sebyone Srl
 *
 * File: daas-types.h
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Disclaimer of Warrant
 * Covered Software is provided under this License on an "as is" basis, without warranty of any kind, either
 * expressed, implied, or statutory, including, without limitation, warranties that the Covered  Software is
 * free of defects, merchantable, fit for a particular purpose or non-infringing.
 * The entire risk as to the quality and performance of the Covered Software is with You.  Should any Covered
 * Software prove defective in any respect, You (not any Contributor) assume the cost of any necessary
 * servicing, repair, or correction.
 * This disclaimer of warranty constitutes an essential part of this License.  No use of any Covered Software
 * is authorized under this License except under this disclaimer.
 *
 * Limitation of Liability
 * Under no circumstances and under no legal theory, whether tort (including negligence), contract, or otherwise,
 * shall any Contributor, or anyone who distributes Covered Software as permitted above, be liable to You for
 * any direct, indirect, special, incidental, or consequential damages of any character including, without
 * limitation, damages for lost profits, loss of goodwill, work stoppage, computer failure or malfunction,
 * or any and all other commercial damages or losses, even if such party shall have been informed of the
 * possibility of such damages.  This limitation of liability shall not apply to liability for death or personal
 * injury resulting from such party's negligence to the extent applicable law prohibits such limitation.
 * Some jurisdictions do not allow the exclusion or limitation of incidental or consequential damages, so this
 * exclusion and limitation may not apply to You.
 *
 * Contributors:
 * plogiacco@smartlab.it - initial design, implementation and documentation
 * sebastiano.meduri@gmail.com  - initial design, implementation and documentation
 * lgrillo@sebyone.com  - implementation and documentation
 *
 */

#ifndef _DAAS_TYPES_H__
#define _DAAS_TYPES_H__

#include <cstdint>

typedef unsigned long long stime_t; // Time-stamp absolute date (64bit) !!!!!!!!!!!!!!! SYSTEM RESOURCE
typedef unsigned long din_t;        // DIN (32bit, 0 NULL,  )
typedef unsigned short typeset_t;   // Typeset (16bit)

#ifndef errno_t
typedef int errno_t;
#endif

class DDO
{
public:
    DDO();
    DDO(typeset_t typeset_);
    DDO(typeset_t typeset_, stime_t timestamp_);
    DDO(const DDO &ddo_);
    ~DDO();

    DDO *getDDO() { return this; }
    //
    void setTimestamp(stime_t tstamp);
    stime_t getTimestamp();
    void setOrigin(din_t);
    din_t getOrigin();
    void setTypeset(typeset_t);
    typeset_t getTypeset();

    uint32_t setPayload(const void *data_, uint32_t size_);
    uint32_t getPayloadSize(void) { return _size; }

    uint32_t getPayloadAsBinary(unsigned char *pbuffer_, unsigned offset_, uint32_t maxSize_);
    inline unsigned char *getPayloadPtr() { return _payload; }

    void clearPayload();

private:
    din_t _origin = 0;      // DIN origin (32bit)
    typeset_t _typeset = 0; // Typeset (16bit)
    stime_t _timestamp = 0; // Time-stamp absolute system's date and time (64bit)
    unsigned char *_payload = nullptr;
    uint32_t _size = 0;
};

template <typename T>
class Vector
{
private:
    T*            m_data;      
    unsigned int  m_size;      
    unsigned int  m_capacity;

public:
    Vector();

    explicit Vector(unsigned int initialCapacity);

    ~Vector();

    unsigned int size() const;

    unsigned int capacity() const;

    bool empty() const;

    bool push_back(const T& value);

    bool pop_back();

    T& operator[](unsigned int index);

    const T& operator[](unsigned int index) const;

    T& at(unsigned int idx);

    void clear();

    // Riserva spazio per almeno newCapacity elementi
    // Restituisce true se riesce, false se allocazione fallisce
    bool reserve(unsigned int newCapacity);
};

class IDaasApiEvent
{
public:
    virtual void dinAcceptedEvent(din_t) = 0;
    virtual void ddoReceivedEvent(int payload_size, typeset_t, din_t) = 0;
};

class DaaSEvent
{
public:
    virtual int daasEvent(int, int, int, int) = 0;
};

struct _DDO_RQ_FETCH_DIN
{
    din_t din_investigating;
    din_t din_to_fetch;
    uint8_t ttl;
    //URI uri;
    /// TODO: add queue to keep track of nodes that have already been asked
};

typedef struct
{
    stime_t lasttime;       // time reference
    uint32_t hwver;         // platform (hard-coded)
    unsigned linked;        // channels counter (0=not linked)
    unsigned sync;          // synchronization status (0=not in sync)
    unsigned lock;          // required security policy
    unsigned char sklen;    // security phrase lenght
    unsigned char skey[14]; // security phrase (UTF-8)
    unsigned form;          // data formatting model
    unsigned codec;         // data encryption level
    // Availabe data ??
} nodestate_t;

struct item_value
{
    unsigned key;
    int value;
};

typedef Vector<item_value> apilist_t;
typedef Vector<int> list_element;
typedef Vector<din_t> dinlist_t;      /// Node API !!!!!!!!!!!!!!
typedef Vector<typeset_t> tsetlist_t; /// Node API !!!!!!!!!!!!!!

/* DRIVER TYPES */
typedef enum : unsigned // Supported communications technologies
{
    _LINK_NONE = 0, // ND
    _LINK_DAAS,     // DaaS routing & loopback
    _LINK_INET4,    // Inet/IP
    _LINK_MQTT5,    // MQTT
    _LINK_UART,     // Serial line
} link_t;

typedef enum
{
    PERFORM_CORE_THREAD = 0,
    PERFORM_CORE_NO_THREAD
} performs_mode_t;

typedef enum : unsigned // Supported protocols
{
    _PROTO_NONE = 0, // ND
    _PROTO_FIXHB,    // Simple fix header packet protocol
    _PROTO_FIXHB_V2, // V2 with support to microseconds time-stamp
} protocol_t;

typedef enum
{
    ERROR_NONE = 0,
    ERROR_MODULE_ALREADY_INITIALIZED,
    ERROR_CORE_STOPPED,
    ERROR_CANNOT_INITIALIZE,
    ERROR_CANNOT_CREATE_NODE,
    ERROR_DIN_ALREADY_EXIST,
    ERROR_CANNOT_MAP_NODE,
    ERROR_INVALID_USER_TYPESET,
    ERROR_SEND_DDO,
    ERROR_NO_DDO_PRESENT,
    ERROR_DIN_UNKNOWN,
    ERROR_CHANNEL_FAILURE,
    // core
    ERROR_INVALID_DME,
    ERROR_NOT_IMPLEMENTED,
    ERROR_UNKNOWN

} daas_error_t;

typedef enum : unsigned // modules statistics facts and settings options
{
    _DAAS_NEW_DDO_RECEIVED = 1001, // received a user's DDO from remote node
    _DAAS_NEW_DIN_ACCEPTED = 1002, // accepted request and added new DIN to map  (NODE_ACCEPT_ALL_REQUESTS)
    _DAAS_NEW_FRISBEE_RECEIVED = 1003
} daas_events_t;

typedef enum : typeset_t
{
    _DAAS_DDO_REPLY_MSG = 0x7FFF, // 32.767
    _DAAS_MASK_SYSDDO = 0x8000,   // 32.768 MASK System's DDO
    _DAAS_RQ_REPLY_MSG,           // 32.769 Request Frisbee service
    _DAAS_RQ_FETCH_DIN,           // 32.770 Fetch path to din
    _DAAS_RT_FETCH_DIN,           // 32.771
    _DAAS_RQ_DIN_STATE,           // 32.772 Get node _lstate
    _DAAS_RT_DIN_STATE,           // 32.773
    _DAAS_RT_UNREACHABLE,         // 32.774
} ddosys_t;

enum core_status_t // STOPPED, STARTING, RUNNING, STOPPING
{
    core_starting,
    core_running,
    core_stopping,
    core_stopped
};

#endif // !_DAAS_TYPES_H__