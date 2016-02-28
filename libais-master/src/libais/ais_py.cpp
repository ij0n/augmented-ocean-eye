// TODO(schwehr): check for reference counting leaks
// TODO(schwehr): better error handling for all messages

#include <Python.h>

#include "ais.h"

namespace libais {

PyObject *ais_py_exception;
const char exception_short[] = "DecodeError";
const char exception_name[] = "_ais.DecodeError";

// TODO(schwehr): Write a full module doc string.

// Functional Identifiers (FI) are individual messages within a
// specific DAC.  An FI in one DAC has nothing to do with an FI in
// another DAC.
enum AIS_FI {
  AIS_FI_6_1_0_TEXT = 0,
  AIS_FI_6_1_1_ACK = 1,
  AIS_FI_6_1_2_FI_INTERROGATE = 2,
  AIS_FI_6_1_3_CAPABILITY_INTERROGATE = 3,
  AIS_FI_6_1_4_CAPABILITY_REPLY = 4,
  AIS_FI_6_1_5_ACK = 5,
  AIS_FI_6_1_12_DANGEROUS_CARGO = 12,
  AIS_FI_6_1_14_TIDAL_WINDOW = 14,
  AIS_FI_6_1_16_VTS_TARGET = 16,
  AIS_FI_6_1_18_ENTRY_TIME = 18,
  AIS_FI_6_1_20_BERTHING = 20,
  AIS_FI_6_1_25_DANGEROUS_CARGO = 25,
  AIS_FI_6_1_28_ROUTE = 28,
  AIS_FI_6_1_30_TEXT = 30,
  AIS_FI_6_1_32_TIDAL_WINDOW = 32,
  AIS_FI_6_1_40_PERSONS_ON_BOARD = 40,
  AIS_FI_6_200_21_RIS_VTS_ETA = 21,
  AIS_FI_6_200_22_RIS_VTS_RTA = 22,
  AIS_FI_6_200_55_RIS_VTS_SAR = 55,

  AIS_FI_8_1_0_TEXT = 0,
  AIS_FI_8_1_11_MET_HYDRO = 11,
  AIS_FI_8_1_13_FAIRWAY_CLOSED = 13,
  AIS_FI_8_1_15_SHIP_AND_VOYAGE = 15,
  AIS_FI_8_1_16_PERSONS_ON_BOARD = 16,
  AIS_FI_8_1_17_VTS_TARGET = 17,
  AIS_FI_8_1_19_TRAFFIC_SIGNAL = 19,
  AIS_FI_8_1_21_WEATHER_OBS = 21,
  AIS_FI_8_1_22_AREA_NOTICE = 22,
  AIS_FI_8_1_24_SHIP_AND_VOYAGE = 24,
  AIS_FI_8_1_26_SENSOR = 26,
  AIS_FI_8_1_27_ROUTE = 27,
  AIS_FI_8_1_29_TEXT = 29,
  AIS_FI_8_1_31_MET_HYDRO = 31,
  AIS_FI_8_1_40_PERSONS_ON_BOARD = 40,
  AIS_FI_8_200_10_RIS_SHIP_AND_VOYAGE = 10,
  AIS_FI_8_200_23_RIS_EMMA_WARNING = 23,
  AIS_FI_8_200_24_RIS_WATERLEVEL = 24,
  AIS_FI_8_200_40_RIS_ATON_SIGNAL_STATUS = 40,
  AIS_FI_8_200_55_RIS_PERSONS_ON_BOARD = 50,
  AIS_FI_8_366_22_AREA_NOTICE = 22,  // USCG.
  AIS_FI_8_367_22_AREA_NOTICE = 22,  // USCG.
};

void
DictSafeSetItem(PyObject *dict, const string &key, const long val) {  // NOLINT
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  PyObject *val_obj = PyLong_FromLong(val);
  assert(key_obj);
  assert(val_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
  Py_DECREF(val_obj);
}

void
DictSafeSetItem(PyObject *dict, const string &key, const int val) {
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  PyObject *val_obj = PyLong_FromLong(val);
  assert(key_obj);
  assert(val_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
  Py_DECREF(val_obj);
}

void
DictSafeSetItem(PyObject *dict, const string &key, const string &val) {
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  PyObject *val_obj = PyUnicode_FromString(val.c_str());
  assert(key_obj);
  assert(val_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
  Py_DECREF(val_obj);
}


void
DictSafeSetItem(PyObject *dict, const string &key, const char *val) {
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  PyObject *val_obj = PyUnicode_FromString(val);
  assert(key_obj);
  assert(val_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
  Py_DECREF(val_obj);
}


#if 0
void
DictSafeSetItem(PyObject *dict, const string &key, const bool val) {
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  PyObject *val_obj = PyBool_FromLong(val);
  assert(key_obj);
  assert(val_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
  Py_DECREF(val_obj);
}
#else
void
DictSafeSetItem(PyObject *dict, const string &key, const bool val) {
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  assert(key_obj);
  if (val) {
    PyDict_SetItem(dict, key_obj, Py_True);
  } else {
    PyDict_SetItem(dict, key_obj, Py_False);
  }
  Py_DECREF(key_obj);
}
#endif

void
DictSafeSetItem(PyObject *dict, const string &key, const float val) {
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  PyObject *val_obj = PyFloat_FromDouble(val);
  assert(key_obj);
  assert(val_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
  Py_DECREF(val_obj);
}

// Python Floats are IEE-754 double precision.
void
DictSafeSetItem(PyObject *dict, const string &key, const double val) {
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  PyObject *val_obj = PyFloat_FromDouble(val);
  assert(key_obj);
  assert(val_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
  Py_DECREF(val_obj);
}

void
DictSafeSetItem(PyObject *dict, const string &x_key, const string& y_key,
                const AisPoint& position) {
  DictSafeSetItem(dict, x_key, position.lng_deg);
  DictSafeSetItem(dict, y_key, position.lat_deg);
}

void
DictSafeSetItem(PyObject *dict, const string &key, PyObject *val_obj) {
  // When we need to add dictionaries and such to a dictionary
  assert(dict);
  assert(val_obj);
  PyObject *key_obj = PyUnicode_FromString(key.c_str());
  assert(key_obj);
  PyDict_SetItem(dict, key_obj, val_obj);
  Py_DECREF(key_obj);
}


PyObject *
ais_msg_to_pydict(const AisMsg* msg) {
  assert(msg);

  PyObject *dict = PyDict_New();
  DictSafeSetItem(dict, "id", msg->message_id);
  DictSafeSetItem(dict, "repeat_indicator", msg->repeat_indicator);
  DictSafeSetItem(dict, "mmsi", msg->mmsi);

  return dict;
}

// Class A position report
PyObject *
ais1_2_3_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);

  Ais1_2_3 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais1_2_3: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "nav_status", msg.nav_status);
  DictSafeSetItem(dict, "rot_over_range", msg.rot_over_range);
  DictSafeSetItem(dict, "rot", msg.rot);
  DictSafeSetItem(dict, "sog", msg.sog);
  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "cog", msg.cog);
  DictSafeSetItem(dict, "true_heading", msg.true_heading);
  DictSafeSetItem(dict, "timestamp", msg.timestamp);
  DictSafeSetItem(dict, "special_manoeuvre", msg.special_manoeuvre);
  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "raim", msg.raim);

  // COMM States
  DictSafeSetItem(dict, "sync_state", msg.sync_state);  // Both SOTDMA & ITDMA

  // SOTDMA
  if (msg.message_id == 1 || msg.message_id == 2) {
    if (msg.slot_timeout_valid) {
      DictSafeSetItem(dict, "slot_timeout", msg.slot_timeout);
    }

    if (msg.received_stations_valid) {
      DictSafeSetItem(dict, "received_stations", msg.received_stations);
    }
    if (msg.slot_number_valid) {
      DictSafeSetItem(dict, "slot_number", msg.slot_number);
    }
    if (msg.utc_valid) {
      DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
      DictSafeSetItem(dict, "utc_min", msg.utc_min);
      DictSafeSetItem(dict, "utc_spare", msg.utc_spare);
    }

    if (msg.slot_offset_valid) {
      DictSafeSetItem(dict, "slot_offset", msg.slot_offset);
    }
  }

  // ITDMA
  if (msg.slot_increment_valid) {
    DictSafeSetItem(dict, "slot_increment", msg.slot_increment);
    DictSafeSetItem(dict, "slots_to_allocate", msg.slots_to_allocate);
    DictSafeSetItem(dict, "keep_flag", msg.keep_flag);
  }

  return dict;
}

// Basestation report and ';' time report
PyObject *
ais4_11_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);

  Ais4_11 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais4_11: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "year", msg.year);
  DictSafeSetItem(dict, "month", msg.month);
  DictSafeSetItem(dict, "day", msg.day);
  DictSafeSetItem(dict, "hour", msg.hour);
  DictSafeSetItem(dict, "minute", msg.minute);
  DictSafeSetItem(dict, "second", msg.second);

  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "x", "y", msg.position);

  DictSafeSetItem(dict, "fix_type", msg.fix_type);
  DictSafeSetItem(dict, "transmission_ctl", msg.transmission_ctl);
  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "raim", msg.raim);

  // SOTDMA
  DictSafeSetItem(dict, "sync_state", msg.sync_state);
  DictSafeSetItem(dict, "slot_timeout", msg.slot_timeout);

  if (msg.received_stations_valid)
    DictSafeSetItem(dict, "received_stations", msg.received_stations);
  if (msg.slot_number_valid)
    DictSafeSetItem(dict, "slot_number", msg.slot_number);
  if (msg.utc_valid) {
    DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
    DictSafeSetItem(dict, "utc_min", msg.utc_min);
    DictSafeSetItem(dict, "utc_spare", msg.utc_spare);
  }

  if (msg.slot_offset_valid)
    DictSafeSetItem(dict, "slot_offset", msg.slot_offset);

  return dict;
}

// Class A ship data
PyObject *
ais5_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais5 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais5: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "ais_version", msg.ais_version);
  DictSafeSetItem(dict, "imo_num", msg.imo_num);
  DictSafeSetItem(dict, "callsign", msg.callsign);
  DictSafeSetItem(dict, "name", msg.name);
  DictSafeSetItem(dict, "type_and_cargo", msg.type_and_cargo);
  DictSafeSetItem(dict, "dim_a", msg.dim_a);
  DictSafeSetItem(dict, "dim_b", msg.dim_b);
  DictSafeSetItem(dict, "dim_c", msg.dim_c);
  DictSafeSetItem(dict, "dim_d", msg.dim_d);
  DictSafeSetItem(dict, "fix_type", msg.fix_type);
  DictSafeSetItem(dict, "eta_month", msg.eta_month);
  DictSafeSetItem(dict, "eta_day", msg.eta_day);
  DictSafeSetItem(dict, "eta_hour", msg.eta_hour);
  DictSafeSetItem(dict, "eta_minute", msg.eta_minute);
  DictSafeSetItem(dict, "draught", msg.draught);
  DictSafeSetItem(dict, "destination", msg.destination);
  DictSafeSetItem(dict, "dte", msg.dte);
  DictSafeSetItem(dict, "spare", msg.spare);

  return dict;
}

// Address binary message
AIS_STATUS
ais6_1_0_append_pydict(const char *nmea_payload, PyObject *dict,
                       const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_0 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "ack_required",  msg.ack_required);
  DictSafeSetItem(dict, "msg_seq",  msg.msg_seq);
  DictSafeSetItem(dict, "text", msg.text);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

AIS_STATUS
ais6_1_1_append_pydict(const char *nmea_payload, PyObject *dict,
                       const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_1 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "ack_dac", msg.ack_dac);
  DictSafeSetItem(dict, "msg_seq", msg.msg_seq);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

AIS_STATUS
ais6_1_2_append_pydict(const char *nmea_payload, PyObject *dict,
                       const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_2 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "req_dac", msg.req_dac);
  DictSafeSetItem(dict, "req_fi", msg.req_fi);

  return AIS_OK;
}

AIS_STATUS
ais6_1_3_append_pydict(const char *nmea_payload, PyObject *dict,
                       const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);

  Ais6_1_3 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "req_dac", msg.req_dac);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

AIS_STATUS
ais6_1_4_append_pydict(const char *nmea_payload, PyObject *dict,
                       const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_4 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "ack_dac", msg.ack_dac);
  PyObject *cap_list = PyList_New(26);
  PyObject *res_list = PyList_New(26);
  for (size_t cap_num = 0; cap_num < 128/2; cap_num++) {
    // TODO(schwehr): memory leak?
    PyObject *cap = PyLong_FromLong(long(msg.capabilities[cap_num]));  // NOLINT
    PyList_SetItem(cap_list, cap_num, cap);

    PyObject *res = PyLong_FromLong(long(msg.cap_reserved[cap_num]));  // NOLINT
    PyList_SetItem(res_list, cap_num, res);
  }
  DictSafeSetItem(dict, "capabilities", cap_list);
  DictSafeSetItem(dict, "cap_reserved", res_list);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

AIS_STATUS
ais6_1_5_append_pydict(const char *nmea_payload, PyObject *dict,
                       const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_5 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "ack_dac", msg.ack_dac);
  DictSafeSetItem(dict, "ack_fi", msg.ack_dac);
  DictSafeSetItem(dict, "seq_num", msg.ack_dac);
  DictSafeSetItem(dict, "ai_available", msg.ack_dac);
  DictSafeSetItem(dict, "ai_response", msg.ack_dac);
  DictSafeSetItem(dict, "spare", msg.ack_dac);

  return AIS_OK;
}

AIS_STATUS
ais6_1_12_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(pad < 6);
  Ais6_1_12 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "last_port", msg.last_port);
  DictSafeSetItem(dict, "utc_month_dep", msg.utc_month_dep);  // actual
  DictSafeSetItem(dict, "utc_day_dep", msg.utc_day_dep);
  DictSafeSetItem(dict, "utc_hour_dep", msg.utc_hour_dep);
  DictSafeSetItem(dict, "utc_min_dep", msg.utc_min_dep);
  DictSafeSetItem(dict, "next_port", msg.next_port);
  DictSafeSetItem(dict, "utc_month_next", msg.utc_month_next);  // estimated
  DictSafeSetItem(dict, "utc_day_next", msg.utc_day_next);
  DictSafeSetItem(dict, "utc_hour_next", msg.utc_hour_next);
  DictSafeSetItem(dict, "utc_min_next", msg.utc_min_next);
  DictSafeSetItem(dict, "main_danger", msg.main_danger);
  DictSafeSetItem(dict, "imo_cat", msg.imo_cat);
  DictSafeSetItem(dict, "un", msg.un);
  DictSafeSetItem(dict, "value", msg.value);  // TODO(schwehr): units?
  DictSafeSetItem(dict, "value_unit", msg.value_unit);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

// 6_1_13 does not exist

// IMO Circ 289 - Tidal Window
AIS_STATUS
ais6_1_14_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(pad < 6);
  Ais6_1_14 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "utc_month", msg.utc_month);
  DictSafeSetItem(dict, "utc_day", msg.utc_day);

  if (!msg.windows.size())
    return AIS_ERR_BAD_SUB_MSG;

  PyObject *window_list = PyList_New(msg.windows.size());
  for (size_t w_num = 0; w_num < msg.windows.size(); w_num++) {
    PyObject *window = PyDict_New();
    DictSafeSetItem(window, "x", "y", msg.windows[w_num].position);
    DictSafeSetItem(window, "utc_hour_from", msg.windows[w_num].utc_hour_from);
    DictSafeSetItem(window, "utc_min_from", msg.windows[w_num].utc_min_from);
    DictSafeSetItem(window, "utc_hour_to", msg.windows[w_num].utc_hour_to);
    DictSafeSetItem(window, "utc_min_to", msg.windows[w_num].utc_min_to);
    DictSafeSetItem(window, "cur_dir", msg.windows[w_num].cur_dir);
    DictSafeSetItem(window, "cur_speed", msg.windows[w_num].cur_speed);
    PyList_SetItem(window_list, w_num, window);
  }
  PyDict_SetItem(dict, PyUnicode_FromString("windows"), window_list);

  return AIS_OK;
}

// 6_1_15, 6_1_16, and 6_1_17 do not exist

// IMO Circ 289 - Clearance time to enter port
AIS_STATUS
ais6_1_18_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(pad < 6);
  Ais6_1_18 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "utc_month", msg.utc_month);
  DictSafeSetItem(dict, "utc_day", msg.utc_day);
  DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
  DictSafeSetItem(dict, "utc_min", msg.utc_min);
  DictSafeSetItem(dict, "port_berth", msg.port_berth);
  DictSafeSetItem(dict, "dest", msg.dest);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "spare2_0", msg.spare2[0]);
  DictSafeSetItem(dict, "spare2_1", msg.spare2[1]);

  return AIS_OK;
}

// 6_1_19 does not exist

// IMO Circ 289 - Berthing data
AIS_STATUS
ais6_1_20_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(pad < 6);
  Ais6_1_20 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "length", msg.length);
  DictSafeSetItem(dict, "depth", msg.depth);
  DictSafeSetItem(dict, "position", msg.mooring_position);
  DictSafeSetItem(dict, "utc_month", msg.utc_month);
  DictSafeSetItem(dict, "utc_day", msg.utc_day);
  DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
  DictSafeSetItem(dict, "utc_min", msg.utc_min);
  if (msg.services_known) {
    PyObject *serv_list = PyList_New(26);
    for (size_t serv_num = 0; serv_num < 26; serv_num++) {
      PyObject *serv = PyLong_FromLong(long(msg.services[serv_num]));  // NOLINT
      PyList_SetItem(serv_list, serv_num, serv);
    }
    DictSafeSetItem(dict, "services", serv_list);
  }
  DictSafeSetItem(dict, "name", msg.name);
  DictSafeSetItem(dict, "x", "y", msg.position);

  return AIS_OK;
}

// 6_1_21, 6_1_22, 6_1_23 and 6_1_24 Do not exist (yet?)

AIS_STATUS
ais6_1_25_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_25 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "amount_unit", msg.amount_unit);
  DictSafeSetItem(dict, "amount", msg.amount);

  if (0 == msg.cargos.size())
    return AIS_ERR_BAD_SUB_MSG;

  PyObject *cargo_list = PyList_New(msg.cargos.size());
  for (size_t cargo_num = 0; cargo_num < msg.cargos.size(); cargo_num++) {
    PyObject *cargo = PyDict_New();
    if (msg.cargos[cargo_num].imdg_valid)
      DictSafeSetItem(cargo, "imdg", msg.cargos[cargo_num].imdg);
    if (msg.cargos[cargo_num].spare_valid)
      DictSafeSetItem(cargo, "spare", msg.cargos[cargo_num].spare);
    if (msg.cargos[cargo_num].un_valid)
      DictSafeSetItem(cargo, "un", msg.cargos[cargo_num].un);
    if (msg.cargos[cargo_num].bc_valid)
      DictSafeSetItem(cargo, "bc", msg.cargos[cargo_num].bc);
    if (msg.cargos[cargo_num].marpol_oil_valid)
      DictSafeSetItem(cargo, "marpol_oil", msg.cargos[cargo_num].marpol_oil);
    if (msg.cargos[cargo_num].marpol_cat_valid)
      DictSafeSetItem(cargo, "marpol_cat", msg.cargos[cargo_num].marpol_cat);
    PyList_SetItem(cargo_list, cargo_num, cargo);
  }
  PyDict_SetItem(dict, PyUnicode_FromString("cargos"), cargo_list);

  return AIS_OK;
}

// 6_1_26
// 6_1_27
// 6_1_28 - TODO(schwehr): Route Addressed - clone from 8_1_27
// 6_1_29
// 6_1_30 - TODO(schwehr): Text Addressed
// 6_1_31

AIS_STATUS
ais6_1_32_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_32 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "utc_month", msg.utc_month);
  DictSafeSetItem(dict, "utc_day", msg.utc_day);

  PyObject *window_list = PyList_New(msg.windows.size());
  for (size_t win_num = 0; win_num < msg.windows.size(); win_num++) {
    PyObject *win = PyDict_New();
    DictSafeSetItem(win, "x", "y", msg.windows[win_num].position);
    DictSafeSetItem(win, "from_utc_hour", msg.windows[win_num].from_utc_hour);
    DictSafeSetItem(win, "from_utc_min", msg.windows[win_num].from_utc_min);
    DictSafeSetItem(win, "to_utc_hour", msg.windows[win_num].to_utc_hour);
    DictSafeSetItem(win, "to_utc_min", msg.windows[win_num].to_utc_min);
    DictSafeSetItem(win, "cur_dir", msg.windows[win_num].cur_dir);
    DictSafeSetItem(win, "cur_speed", msg.windows[win_num].cur_speed);
    PyList_SetItem(window_list, win_num, win);
  }
  PyDict_SetItem(dict, PyUnicode_FromString("windows"), window_list);

  return AIS_OK;
}

AIS_STATUS
ais6_1_40_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais6_1_40 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }
  DictSafeSetItem(dict, "persons", msg.persons);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

PyObject*
ais6_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais6 msg(nmea_payload, pad);
  if (msg.had_error() && msg.get_error() != AIS_UNINITIALIZED) {
    PyErr_Format(ais_py_exception, "Ais6: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "seq", msg.seq);
  DictSafeSetItem(dict, "mmsi_dest", msg.mmsi_dest);
  DictSafeSetItem(dict, "retransmit", msg.retransmit);
  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "dac", msg.dac);
  DictSafeSetItem(dict, "fi", msg.fi);

  // TODO(schwehr): manage all the submessage types

  AIS_STATUS status = AIS_UNINITIALIZED;

  switch (msg.dac) {
  case AIS_DAC_1_INTERNATIONAL:  // IMO.
    switch (msg.fi) {
    case AIS_FI_6_1_0_TEXT:  // OLD ITU 1371-1.
      status = ais6_1_0_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_1_ACK:  // OLD ITU 1371-1.
      status = ais6_1_1_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_2_FI_INTERROGATE:  // OLD ITU 1371-1.
      status = ais6_1_2_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_3_CAPABILITY_INTERROGATE:  // OLD ITU 1371-1.
      status = ais6_1_3_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_4_CAPABILITY_REPLY:  // OLD ITU 1371-1.
      status = ais6_1_4_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_5_ACK:  // ITU 1371-5.
      status = ais6_1_5_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_12_DANGEROUS_CARGO:  // Not to be used after 1 Jan 2013.
      status = ais6_1_12_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_14_TIDAL_WINDOW:  // Not to be used after 1 Jan 2013.
      status = ais6_1_14_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_18_ENTRY_TIME:
      status = ais6_1_18_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_20_BERTHING:
      status = ais6_1_20_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_25_DANGEROUS_CARGO:
      status = ais6_1_25_append_pydict(nmea_payload, dict, pad);
      break;
      // TODO(schwehr): AIS_FI_6_1_28_ROUTE.
      // TODO(schwehr): AIS_FI_6_1_30_TEXT.
    case AIS_FI_6_1_32_TIDAL_WINDOW:  // IMO Circ 289
      status = ais6_1_32_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_6_1_40_PERSONS_ON_BOARD:  // OLD ITU 1371-1.
      status = ais6_1_40_append_pydict(nmea_payload, dict, pad);
      break;
    default:
      // TODO(schwehr): Raise an exception?
      DictSafeSetItem(dict, "not_parsed", true);
    }
    break;

    default:
      // TODO(schwehr): Raise an exception?
      DictSafeSetItem(dict, "not_parsed", true);
  }

  if (status != AIS_OK) {
    PyErr_Format(ais_py_exception,
                 "Ais6: DAC:FI not known.  6:%d:%d %s",
                 msg.dac,
                 msg.fi,
                 AIS_STATUS_STRINGS[status]);
    return nullptr;
  }

  return dict;
}

// Acknowledgement
PyObject*
ais7_13_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais7_13 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais7_13: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  PyObject *list = PyList_New(msg.dest_mmsi.size());
  for (size_t i = 0; i < msg.dest_mmsi.size(); i++) {
    PyObject *tuple = PyTuple_New(2);
    PyTuple_SetItem(tuple, 0, PyLong_FromLong(msg.dest_mmsi[i]));  // Steals ref
    PyTuple_SetItem(tuple, 1, PyLong_FromLong(msg.seq_num[i]));  // Steals ref
    PyList_SetItem(list, i, tuple);  // Steals ref
  }
  PyDict_SetItem(dict, PyUnicode_FromString("acks"), list);
  Py_DECREF(list);
  return dict;
}

AIS_STATUS
ais8_1_0_append_pydict(const char *nmea_payload, PyObject *dict,
                       const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_0 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "ack_required",  msg.ack_required);
  DictSafeSetItem(dict, "msg_seq",  msg.msg_seq);
  DictSafeSetItem(dict, "text", msg.text);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

// ais 8_1_[1..10] do not exist

AIS_STATUS
ais8_1_11_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_11 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "x", "y", msg.position);

  DictSafeSetItem(dict, "wind_ave", msg.wind_ave);
  DictSafeSetItem(dict, "wind_gust", msg.wind_gust);
  DictSafeSetItem(dict, "wind_dir", msg.wind_dir);
  DictSafeSetItem(dict, "wind_gust_dir", msg.wind_gust);

  DictSafeSetItem(dict, "air_temp", msg.air_temp);
  DictSafeSetItem(dict, "rel_humid", msg.rel_humid);
  DictSafeSetItem(dict, "dew_point", msg.dew_point);
  DictSafeSetItem(dict, "air_pres", msg.air_pres);
  DictSafeSetItem(dict, "air_pres_trend", msg.air_pres_trend);
  DictSafeSetItem(dict, "horz_vis", msg.horz_vis);

  DictSafeSetItem(dict, "water_level", msg.water_level);
  DictSafeSetItem(dict, "water_level_trend", msg.water_level_trend);

  DictSafeSetItem(dict, "surf_cur_speed", msg.surf_cur_speed);
  DictSafeSetItem(dict, "surf_cur_dir", msg.surf_cur_dir);

  DictSafeSetItem(dict, "cur_speed_2", msg.cur_speed_2);
  DictSafeSetItem(dict, "cur_dir_2",   msg.cur_dir_2);
  DictSafeSetItem(dict, "cur_depth_2", msg.cur_depth_2);

  DictSafeSetItem(dict, "cur_speed_3", msg.cur_speed_3);
  DictSafeSetItem(dict, "cur_dir_3",   msg.cur_dir_3);
  DictSafeSetItem(dict, "cur_depth_3", msg.cur_depth_3);

  DictSafeSetItem(dict, "wave_height", msg.wave_height);
  DictSafeSetItem(dict, "wave_period", msg.wave_period);
  DictSafeSetItem(dict, "wave_dir", msg.wave_dir);

  DictSafeSetItem(dict, "swell_height", msg.swell_height);
  DictSafeSetItem(dict, "swell_period", msg.swell_period);
  DictSafeSetItem(dict, "swell_dir", msg.swell_dir);

  DictSafeSetItem(dict, "sea_state", msg.sea_state);
  DictSafeSetItem(dict, "water_temp", msg.water_temp);
  DictSafeSetItem(dict, "precip_type", msg.precip_type);
  DictSafeSetItem(dict, "ice", msg.ice);  // Grr... ice

  // Or could be spare
  DictSafeSetItem(dict, "ext_water_level", msg.extended_water_level);
  DictSafeSetItem(dict, "spare2", msg.extended_water_level);

  return AIS_OK;
}

// 12 is addressed

AIS_STATUS
ais8_1_13_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_13 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "reason", msg.reason);
  DictSafeSetItem(dict, "location_from", msg.location_from);
  DictSafeSetItem(dict, "location_to", msg.location_to);
  DictSafeSetItem(dict, "radius", msg.radius);
  DictSafeSetItem(dict, "units", msg.units);
  DictSafeSetItem(dict, "day_from", msg.day_from);
  DictSafeSetItem(dict, "month_from", msg.month_from);
  DictSafeSetItem(dict, "hour_from", msg.hour_from);
  DictSafeSetItem(dict, "minute_from", msg.minute_from);
  DictSafeSetItem(dict, "day_to", msg.day_to);
  DictSafeSetItem(dict, "month_to", msg.month_to);
  DictSafeSetItem(dict, "hour_to", msg.hour_to);
  DictSafeSetItem(dict, "minute_to", msg.minute_to);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}

// 14 is addressed

AIS_STATUS
ais8_1_15_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_15 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "air_draught", msg.air_draught);
  DictSafeSetItem(dict, "spare2", msg.spare2);
  return AIS_OK;
}


AIS_STATUS
ais8_1_16_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_16 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "persons", msg.persons);
  DictSafeSetItem(dict, "spare2", msg.spare2);
  return AIS_OK;
}


AIS_STATUS
ais8_1_17_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_17 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  PyObject *target_list = PyList_New(msg.targets.size());
  for (size_t target_num = 0; target_num < msg.targets.size(); target_num++) {
    PyObject *target = PyDict_New();
    DictSafeSetItem(target, "type", msg.targets[target_num].type);
    DictSafeSetItem(target, "id", msg.targets[target_num].id);
    DictSafeSetItem(target, "spare", msg.targets[target_num].spare);
    DictSafeSetItem(target, "x", "y", msg.targets[target_num].position);
    DictSafeSetItem(target, "cog", msg.targets[target_num].cog);
    DictSafeSetItem(target, "timestamp", msg.targets[target_num].timestamp);
    DictSafeSetItem(target, "sog", msg.targets[target_num].sog);
    PyList_SetItem(target_list, target_num, target);
  }
  PyDict_SetItem(dict, PyUnicode_FromString("targets"), target_list);

  return AIS_OK;
}

// 18 is addressed only

AIS_STATUS
ais8_1_19_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_19 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "name", msg.name);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "status", msg.status);
  DictSafeSetItem(dict, "signal", msg.signal);
  DictSafeSetItem(dict, "utc_hour_next", msg.utc_hour_next);
  DictSafeSetItem(dict, "utc_min_next", msg.utc_min_next);
  DictSafeSetItem(dict, "next_signal", msg.next_signal);
  DictSafeSetItem(dict, "spare2_0", msg.spare2[0]);
  DictSafeSetItem(dict, "spare2_1", msg.spare2[1]);
  DictSafeSetItem(dict, "spare2_2", msg.spare2[2]);
  DictSafeSetItem(dict, "spare2_3", msg.spare2[3]);

  return AIS_OK;
}

// 20 is addressed

AIS_STATUS
ais8_1_21_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_21 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "utc_day", msg.utc_day);
  DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
  DictSafeSetItem(dict, "utc_min", msg.utc_min);

  if (0 == msg.type_wx_report) {
    // WX obs from ship
    DictSafeSetItem(dict, "location", msg.location);
    DictSafeSetItem(dict, "wx", msg.wx[0]);  // TODO(schwehr) Rename present?
    DictSafeSetItem(dict, "horz_viz", msg.horz_viz);
    DictSafeSetItem(dict, "humidity", msg.humidity);
    DictSafeSetItem(dict, "wind_speed", msg.wind_speed);
    DictSafeSetItem(dict, "wind_dir", msg.wind_dir);
    DictSafeSetItem(dict, "pressure", msg.pressure);
    DictSafeSetItem(dict, "pressure_tendency", msg.pressure_tendency);
    DictSafeSetItem(dict, "air_temp", msg.air_temp);
    DictSafeSetItem(dict, "water_temp", msg.water_temp);
    DictSafeSetItem(dict, "wave_period", msg.wave_period);
    DictSafeSetItem(dict, "wave_height", msg.wave_height);
    DictSafeSetItem(dict, "wave_dir", msg.wave_dir);
    DictSafeSetItem(dict, "swell_height", msg.swell_height);
    DictSafeSetItem(dict, "swell_dir", msg.swell_dir);
    DictSafeSetItem(dict, "swell_period", msg.swell_period);
    DictSafeSetItem(dict, "spare2", msg.spare2);
  } else {
    // type == 1
    // WMO OBS from ship
    DictSafeSetItem(dict, "utc_month", msg.utc_month);
    DictSafeSetItem(dict, "cog", msg.cog);
    DictSafeSetItem(dict, "sog", msg.sog);
    DictSafeSetItem(dict, "heading", msg.heading);
    DictSafeSetItem(dict, "pressure", msg.pressure);
    DictSafeSetItem(dict, "rel_pressure", msg.rel_pressure);
    DictSafeSetItem(dict, "pressure_tendency", msg.pressure_tendency);
    DictSafeSetItem(dict, "wind_dir", msg.wind_dir);
    DictSafeSetItem(dict, "wind_speed_ms", msg.wind_speed_ms);
    DictSafeSetItem(dict, "wind_dir_rel", msg.wind_dir_rel);
    DictSafeSetItem(dict, "wind_speed_rel", msg.wind_speed_rel);
    DictSafeSetItem(dict, "wind_gust_speed", msg.wind_gust_speed);
    DictSafeSetItem(dict, "wind_gust_dir", msg.wind_gust_dir);
    DictSafeSetItem(dict, "air_temp_raw", msg.air_temp_raw);
    DictSafeSetItem(dict, "humidity", msg.humidity);
    DictSafeSetItem(dict, "water_temp_raw", msg.water_temp_raw);
    DictSafeSetItem(dict, "horz_viz", msg.horz_viz);
    // TODO(schwehr): list?
    DictSafeSetItem(dict, "wx", msg.wx[0]);
    DictSafeSetItem(dict, "wx_next1", msg.wx[1]);
    DictSafeSetItem(dict, "wx_next2", msg.wx[2]);
    DictSafeSetItem(dict, "cloud_total", msg.cloud_total);
    DictSafeSetItem(dict, "cloud_low", msg.cloud_low);
    DictSafeSetItem(dict, "cloud_low_type", msg.cloud_low_type);
    DictSafeSetItem(dict, "cloud_middle_type", msg.cloud_middle_type);
    DictSafeSetItem(dict, "cloud_high_type", msg.cloud_high_type);
    DictSafeSetItem(dict, "alt_lowest_cloud_base", msg.alt_lowest_cloud_base);
    DictSafeSetItem(dict, "wave_period", msg.wave_period);
    DictSafeSetItem(dict, "wave_height", msg.wave_height);
    DictSafeSetItem(dict, "swell_dir", msg.swell_dir);
    DictSafeSetItem(dict, "swell_period", msg.swell_period);
    DictSafeSetItem(dict, "swell_height", msg.swell_height);
    DictSafeSetItem(dict, "swell_dir_2", msg.swell_dir_2);
    DictSafeSetItem(dict, "swell_period_2", msg.swell_period_2);
    DictSafeSetItem(dict, "swell_height_2", msg.swell_height_2);
    DictSafeSetItem(dict, "ice_thickness", msg.ice_thickness);
    DictSafeSetItem(dict, "ice_accretion", msg.ice_accretion);
    DictSafeSetItem(dict, "ice_accretion_cause", msg.ice_accretion_cause);
    DictSafeSetItem(dict, "sea_ice_concentration", msg.sea_ice_concentration);
    DictSafeSetItem(dict, "amt_type_ice", msg.amt_type_ice);
    DictSafeSetItem(dict, "ice_situation", msg.ice_situation);
    DictSafeSetItem(dict, "ice_devel", msg.ice_devel);
    DictSafeSetItem(dict, "bearing_ice_edge", msg.bearing_ice_edge);
  }

  return AIS_OK;
}


AIS_STATUS
ais8_1_22_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  Ais8_1_22 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "notice_type", msg.notice_type);
  DictSafeSetItem(dict, "notice_type_str",
                  ais8_1_22_notice_names[msg.notice_type]);

  DictSafeSetItem(dict, "month", msg.month);
  DictSafeSetItem(dict, "day", msg.day);
  DictSafeSetItem(dict, "hour", msg.hour);
  DictSafeSetItem(dict, "minute", msg.minute);
  DictSafeSetItem(dict, "duration_minutes", msg.minute);

  PyObject *sub_area_list = PyList_New(msg.sub_areas.size());

  // Loop over sub_areas
  for (size_t i = 0; i < msg.sub_areas.size(); i++) {
    switch (msg.sub_areas[i]->getType()) {
    case AIS8_1_22_SHAPE_CIRCLE:  // or point
      {
        PyObject *sub_area = PyDict_New();
        Ais8_1_22_Circle *c =
            reinterpret_cast<Ais8_1_22_Circle*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_1_22_SHAPE_CIRCLE);
        if (c->radius_m == 0)
          DictSafeSetItem(sub_area, "sub_area_type_str", "point");
        else
          DictSafeSetItem(sub_area, "sub_area_type_str", "circle");

        DictSafeSetItem(sub_area, "x", "y", c->position);
        DictSafeSetItem(sub_area, "precision", c->precision);
        DictSafeSetItem(sub_area, "radius", c->radius_m);
        // TODO(schwehr): spare?
        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_1_22_SHAPE_RECT:
      {
        PyObject *sub_area = PyDict_New();
        Ais8_1_22_Rect *c =
            reinterpret_cast<Ais8_1_22_Rect*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_1_22_SHAPE_RECT);
        DictSafeSetItem(sub_area, "sub_area_type_str", "rect");

        DictSafeSetItem(sub_area, "x", "y", c->position);
        DictSafeSetItem(sub_area, "precision", c->precision);
        DictSafeSetItem(sub_area, "e_dim_m", c->e_dim_m);
        DictSafeSetItem(sub_area, "n_dim_m", c->n_dim_m);
        DictSafeSetItem(sub_area, "orient_deg", c->orient_deg);
        // TODO(schwehr): spare?
        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_1_22_SHAPE_SECTOR:
      {
        PyObject *sub_area = PyDict_New();
        Ais8_1_22_Sector *c =
            reinterpret_cast<Ais8_1_22_Sector*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_1_22_SHAPE_SECTOR);
        DictSafeSetItem(sub_area, "sub_area_type_str", "sector");

        DictSafeSetItem(sub_area, "x", "y", c->position);
        DictSafeSetItem(sub_area, "precision", c->precision);
        DictSafeSetItem(sub_area, "radius", c->radius_m);
        DictSafeSetItem(sub_area, "left_bound_deg", c->left_bound_deg);
        DictSafeSetItem(sub_area, "right_bound_deg", c->right_bound_deg);
        // TODO(schwehr): spare?
        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_1_22_SHAPE_POLYLINE:
      {
        PyObject *sub_area = PyDict_New();
        Ais8_1_22_Polyline *polyline =
            reinterpret_cast<Ais8_1_22_Polyline*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_1_22_SHAPE_POLYLINE);
        DictSafeSetItem(sub_area, "sub_area_type_str", "polyline");
        assert(polyline->angles.size() == polyline->dists_m.size());
        PyObject *angle_list = PyList_New(polyline->angles.size());
        PyObject *dist_list = PyList_New(polyline->angles.size());

        for (size_t pt_num = 0; pt_num < polyline->angles.size(); pt_num++) {
          PyList_SetItem(angle_list, pt_num,
                         PyFloat_FromDouble(polyline->angles[pt_num]));
          PyList_SetItem(dist_list, pt_num,
                         PyFloat_FromDouble(polyline->dists_m[pt_num]));
        }

        DictSafeSetItem(sub_area, "angles", angle_list);
        DictSafeSetItem(sub_area, "dists_m", dist_list);

        // TODO(schwehr): spare?
        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_1_22_SHAPE_POLYGON:
      {
        PyObject *sub_area = PyDict_New();
        Ais8_1_22_Polygon *polygon =
            reinterpret_cast<Ais8_1_22_Polygon*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_1_22_SHAPE_POLYGON);
        DictSafeSetItem(sub_area, "sub_area_type_str", "polygon");
        assert(polygon->angles.size() == polygon->dists_m.size());
        PyObject *angle_list = PyList_New(polygon->angles.size());
        PyObject *dist_list = PyList_New(polygon->angles.size());

        for (size_t pt_num = 0; pt_num < polygon->angles.size(); pt_num++) {
          PyList_SetItem(angle_list, pt_num,
                         PyFloat_FromDouble(polygon->angles[pt_num]));
          PyList_SetItem(dist_list, pt_num,
                         PyFloat_FromDouble(polygon->dists_m[pt_num]));
        }

        DictSafeSetItem(sub_area, "angles", angle_list);
        DictSafeSetItem(sub_area, "dists_m", dist_list);

        // TODO(schwehr): spare?
        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_1_22_SHAPE_TEXT:
      {
        PyObject *sub_area = PyDict_New();

        Ais8_1_22_Text *text =
            reinterpret_cast<Ais8_1_22_Text*>(msg.sub_areas[i]);
        DictSafeSetItem(sub_area, "sub_area_type", AIS8_1_22_SHAPE_TEXT);
        DictSafeSetItem(sub_area, "sub_area_type_str", "text");

        DictSafeSetItem(sub_area, "text", text->text);
        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;

    default:
      {}  // TODO(schwehr): Mark an unknown subarea or raise an exception.
    }
  }
  DictSafeSetItem(dict, "sub_areas", sub_area_list);

  return AIS_OK;
}


// No 23 broadcast

// IMO Circ 289 - Extended ship static and voyage-related
AIS_STATUS
ais8_1_24_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_24 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "air_draught", msg.air_draught);
  DictSafeSetItem(dict, "last_port", msg.last_port);

  PyObject *port_list = PyList_New(2);
  PyObject *port0 = PyUnicode_FromString(msg.next_ports[0].c_str());
  PyObject *port1 = PyUnicode_FromString(msg.next_ports[0].c_str());
  PyList_SetItem(port_list, 0, port0); Py_DECREF(port0);
  PyList_SetItem(port_list, 1, port1); Py_DECREF(port1);

  PyObject *solas_list = PyList_New(26);
  for (size_t solas_num = 0; solas_num < 26; solas_num++) {
    PyObject *solas = PyLong_FromLong(msg.solas_status[solas_num]);
    PyList_SetItem(solas_list, solas_num, solas);
  }
  DictSafeSetItem(dict, "solas", solas_list);
  DictSafeSetItem(dict, "ice_class", msg.ice_class);
  DictSafeSetItem(dict, "shaft_power", msg.shaft_power);
  DictSafeSetItem(dict, "vhf", msg.vhf);
  DictSafeSetItem(dict, "lloyds_ship_type", msg.lloyds_ship_type);
  DictSafeSetItem(dict, "gross_tonnage", msg.gross_tonnage);
  DictSafeSetItem(dict, "laden_ballast", msg.laden_ballast);
  DictSafeSetItem(dict, "heavy_oil", msg.heavy_oil);
  DictSafeSetItem(dict, "light_oil", msg.light_oil);
  DictSafeSetItem(dict, "diesel", msg.diesel);
  DictSafeSetItem(dict, "bunker_oil", msg.bunker_oil);
  DictSafeSetItem(dict, "persons", msg.persons);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}


// No 25 broadcast

AIS_STATUS
ais8_1_26_append_pydict_sensor_hdr(PyObject *dict,
                                   Ais8_1_26_SensorReport* rpt) {
  assert(dict);
  assert(rpt);
  DictSafeSetItem(dict, "report_type", rpt->report_type);
  DictSafeSetItem(dict, "utc_day", rpt->utc_day);
  DictSafeSetItem(dict, "utc_hr", rpt->utc_hr);
  DictSafeSetItem(dict, "utc_min", rpt->utc_min);
  DictSafeSetItem(dict, "site_id", rpt->site_id);

  return AIS_OK;
}


// IMO Circ 289 - Environmental
AIS_STATUS
ais8_1_26_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_26 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  PyObject *rpt_list = PyList_New(msg.reports.size());
  DictSafeSetItem(dict, "reports", rpt_list);

  for (size_t rpt_num = 0; rpt_num < msg.reports.size(); rpt_num++) {
    PyObject *rpt_dict = PyDict_New();
    PyList_SetItem(rpt_list, rpt_num, rpt_dict);

    switch (msg.reports[rpt_num]->report_type) {
      // case AIS8_1_26_SENSOR_ERROR:
    case AIS8_1_26_SENSOR_LOCATION:
      {
        Ais8_1_26_Location *rpt =
            reinterpret_cast<Ais8_1_26_Location *>(msg.reports[rpt_num]);
        ais8_1_26_append_pydict_sensor_hdr(rpt_dict, rpt);
        DictSafeSetItem(rpt_dict, "x", "y", rpt->position);
        DictSafeSetItem(rpt_dict, "z", rpt->z);
        DictSafeSetItem(rpt_dict, "owner", rpt->owner);
        DictSafeSetItem(rpt_dict, "timeout", rpt->timeout);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);
      }
      break;
    case AIS8_1_26_SENSOR_STATION:
      {
        Ais8_1_26_Station *rpt =
            reinterpret_cast<Ais8_1_26_Station *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "name", rpt->name);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);
      }
      break;
    case AIS8_1_26_SENSOR_WIND:
      {
        Ais8_1_26_Wind *rpt =
            reinterpret_cast<Ais8_1_26_Wind *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "wind_speed", rpt->wind_speed);
        DictSafeSetItem(rpt_dict, "wind_gust", rpt->wind_gust);
        DictSafeSetItem(rpt_dict, "wind_dir", rpt->wind_dir);
        DictSafeSetItem(rpt_dict, "wind_gust_dir", rpt->wind_gust_dir);
        DictSafeSetItem(rpt_dict, "sensor_type", rpt->sensor_type);
        DictSafeSetItem(rpt_dict, "wind_forecast", rpt->wind_forecast);
        DictSafeSetItem(
            rpt_dict, "wind_gust_forecast", rpt->wind_gust_forecast);
        DictSafeSetItem(rpt_dict, "wind_dir_forecast", rpt->wind_dir_forecast);
        DictSafeSetItem(rpt_dict, "utc_day_forecast", rpt->utc_day_forecast);
        DictSafeSetItem(rpt_dict, "utc_hour_forecast", rpt->utc_hour_forecast);
        DictSafeSetItem(rpt_dict, "utc_min_forecast", rpt->utc_min_forecast);
        DictSafeSetItem(rpt_dict, "duration", rpt->duration);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);
      }
      break;
    case AIS8_1_26_SENSOR_WATER_LEVEL:
      {
        Ais8_1_26_WaterLevel *rpt =
            reinterpret_cast<Ais8_1_26_WaterLevel *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "type", rpt->type);
        DictSafeSetItem(rpt_dict, "level", rpt->level);
        DictSafeSetItem(rpt_dict, "trend", rpt->trend);
        DictSafeSetItem(rpt_dict, "vdatum", rpt->vdatum);
        DictSafeSetItem(rpt_dict, "sensor_type", rpt->sensor_type);
        DictSafeSetItem(rpt_dict, "forecast_type", rpt->forecast_type);
        DictSafeSetItem(rpt_dict, "level_forecast", rpt->level_forecast);
        DictSafeSetItem(rpt_dict, "utc_day_forecast", rpt->utc_day_forecast);
        DictSafeSetItem(rpt_dict, "utc_hour_forecast", rpt->utc_hour_forecast);
        DictSafeSetItem(rpt_dict, "utc_min_forecast", rpt->utc_min_forecast);
        DictSafeSetItem(rpt_dict, "duration", rpt->duration);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);
      }
      break;
    case AIS8_1_26_SENSOR_CURR_2D:
      {
        Ais8_1_26_Curr2D *rpt =
            reinterpret_cast<Ais8_1_26_Curr2D *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "type", rpt->type);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);

        PyObject *curr_list = PyList_New(3);
        DictSafeSetItem(dict, "currents", curr_list);
        for (size_t idx = 0; idx < 3; idx++) {
          PyObject *curr_dict = PyDict_New();
          DictSafeSetItem(curr_dict, "speed", rpt->currents[idx].speed);
          DictSafeSetItem(curr_dict, "dir", rpt->currents[idx].dir);
          DictSafeSetItem(curr_dict, "depth", rpt->currents[idx].depth);
          PyList_SetItem(curr_list, idx, curr_dict);
        }
      }
      break;
    case AIS8_1_26_SENSOR_CURR_3D:
      {
        Ais8_1_26_Curr3D *rpt =
            reinterpret_cast<Ais8_1_26_Curr3D *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "type", rpt->type);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);

        PyObject *curr_list = PyList_New(3);
        DictSafeSetItem(dict, "currents", curr_list);
        for (size_t idx = 0; idx < 2; idx++) {
          // ERROR: no way to specify negative direction
          PyObject *curr_dict = PyDict_New();
          PyList_SetItem(curr_list, idx, curr_dict);
          DictSafeSetItem(curr_dict, "north", rpt->currents[idx].north);
          DictSafeSetItem(curr_dict, "east", rpt->currents[idx].east);
          DictSafeSetItem(curr_dict, "up", rpt->currents[idx].up);
          DictSafeSetItem(curr_dict, "depth", rpt->currents[idx].depth);
        }
      }
      break;
    case AIS8_1_26_SENSOR_HORZ_FLOW:
      {
        Ais8_1_26_HorzFlow *rpt =
            reinterpret_cast<Ais8_1_26_HorzFlow *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);

        PyObject *curr_list = PyList_New(3);
        DictSafeSetItem(dict, "currents", curr_list);
        for (size_t idx = 0; idx < 2; idx++) {
          PyObject *curr_dict = PyDict_New();
          PyList_SetItem(curr_list, idx, curr_dict);
          DictSafeSetItem(curr_dict, "bearing", rpt->currents[idx].bearing);
          DictSafeSetItem(curr_dict, "dist", rpt->currents[idx].dist);
          DictSafeSetItem(curr_dict, "speed", rpt->currents[idx].speed);
          DictSafeSetItem(curr_dict, "dir", rpt->currents[idx].dir);
          DictSafeSetItem(curr_dict, "level", rpt->currents[idx].level);
        }
      }
      break;
    case AIS8_1_26_SENSOR_SEA_STATE:
      {
        Ais8_1_26_SeaState *rpt =
            reinterpret_cast<Ais8_1_26_SeaState *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "swell_height", rpt->swell_height);
        DictSafeSetItem(rpt_dict, "swell_period", rpt->swell_period);
        DictSafeSetItem(rpt_dict, "swell_dir", rpt->swell_dir);
        DictSafeSetItem(rpt_dict, "sea_state", rpt->sea_state);
        DictSafeSetItem(rpt_dict, "swell_sensor_type", rpt->swell_sensor_type);
        DictSafeSetItem(rpt_dict, "water_temp", rpt->water_temp);
        DictSafeSetItem(rpt_dict, "water_temp_depth", rpt->water_temp_depth);
        DictSafeSetItem(rpt_dict, "water_sensor_type", rpt->water_sensor_type);
        DictSafeSetItem(rpt_dict, "wave_height", rpt->wave_height);
        DictSafeSetItem(rpt_dict, "wave_period", rpt->wave_period);
        DictSafeSetItem(rpt_dict, "wave_dir", rpt->wave_dir);
        DictSafeSetItem(rpt_dict, "wave_sensor_type", rpt->wave_sensor_type);
        DictSafeSetItem(rpt_dict, "salinity", rpt->salinity);
      }
      break;
    case AIS8_1_26_SENSOR_SALINITY:
      {
        Ais8_1_26_Salinity *rpt =
            reinterpret_cast<Ais8_1_26_Salinity *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "water_temp", rpt->water_temp);
        DictSafeSetItem(rpt_dict, "conductivity", rpt->conductivity);
        DictSafeSetItem(rpt_dict, "pressure", rpt->pressure);
        DictSafeSetItem(rpt_dict, "salinity", rpt->salinity);
        DictSafeSetItem(rpt_dict, "salinity_type", rpt->salinity_type);
        DictSafeSetItem(rpt_dict, "sensor_type", rpt->sensor_type);
        DictSafeSetItem(rpt_dict, "spare0", rpt->spare[0]);
        DictSafeSetItem(rpt_dict, "spare1", rpt->spare[1]);
      }
      break;
    case AIS8_1_26_SENSOR_WX:
      {
        Ais8_1_26_Wx *rpt =
            reinterpret_cast<Ais8_1_26_Wx *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "air_temp", rpt->air_temp);
        DictSafeSetItem(rpt_dict, "air_temp_sensor_type",
                        rpt->air_temp_sensor_type);
        DictSafeSetItem(rpt_dict, "precip", rpt->precip);
        DictSafeSetItem(rpt_dict, "horz_vis", rpt->horz_vis);
        DictSafeSetItem(rpt_dict, "dew_point", rpt->dew_point);
        DictSafeSetItem(rpt_dict, "dew_point_type", rpt->dew_point_type);
        DictSafeSetItem(rpt_dict, "air_pressure", rpt->air_pressure);
        DictSafeSetItem(rpt_dict, "air_pressure_trend",
                        rpt->air_pressure_trend);
        DictSafeSetItem(rpt_dict, "air_pressor_type", rpt->air_pressor_type);
        DictSafeSetItem(rpt_dict, "salinity", rpt->salinity);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);
      }
      break;
    case AIS8_1_26_SENSOR_AIR_DRAUGHT:
      {
        Ais8_1_26_AirDraught *rpt =
            reinterpret_cast<Ais8_1_26_AirDraught *>(msg.reports[rpt_num]);
        DictSafeSetItem(rpt_dict, "draught", rpt->draught);
        DictSafeSetItem(rpt_dict, "gap", rpt->gap);
        DictSafeSetItem(rpt_dict, "forecast_gap", rpt->forecast_gap);
        DictSafeSetItem(rpt_dict, "int trend", rpt->trend);
        DictSafeSetItem(
            rpt_dict, "int utc_day_forecast", rpt->utc_day_forecast);
        DictSafeSetItem(rpt_dict, "utc_hour_forecast", rpt->utc_hour_forecast);
        DictSafeSetItem(rpt_dict, "utc_min_forecast", rpt->utc_min_forecast);
        DictSafeSetItem(rpt_dict, "spare", rpt->spare);
      }
      break;
    case AIS8_1_26_SENSOR_RESERVED_11:  // FALLTHROUGH
    case AIS8_1_26_SENSOR_RESERVED_12:
    case AIS8_1_26_SENSOR_RESERVED_13:
    case AIS8_1_26_SENSOR_RESERVED_14:
    case AIS8_1_26_SENSOR_RESERVED_15:
    default:
      {}  // TODO(schwehr): mark a bad sensor type or raise exception
    }
  }

  return AIS_OK;
}


// IMO Circ 289 - Route information
AIS_STATUS
ais8_1_27_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_27 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "sender_type", msg.sender_type);
  DictSafeSetItem(dict, "route_type", msg.route_type);
  DictSafeSetItem(dict, "utc_month", msg.utc_month);
  DictSafeSetItem(dict, "utc_day", msg.utc_day);
  DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
  DictSafeSetItem(dict, "utc_min", msg.utc_min);
  DictSafeSetItem(dict, "duration", msg.duration);

  PyObject *waypoint_list = PyList_New(msg.waypoints.size());
  for (size_t point_num = 0; point_num < msg.waypoints.size(); point_num++) {
    PyObject *waypoint = PyList_New(2);
    // TODO(schwehr): Py_DECREF(); ?
    PyList_SetItem(
        waypoint, 0, PyFloat_FromDouble(msg.waypoints[point_num].lng_deg));
    PyList_SetItem(
        waypoint, 1, PyFloat_FromDouble(msg.waypoints[point_num].lat_deg));
    PyList_SetItem(waypoint_list, point_num, waypoint);
  }
  PyDict_SetItem(dict, PyUnicode_FromString("waypoints"), waypoint_list);

  return AIS_OK;
}


// no 28 broadcast

// IMO Circ 289 - Text description
AIS_STATUS
ais8_1_29_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_29 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "text", msg.text);

  return AIS_OK;
}


// no 30 broadcast

AIS_STATUS
ais8_1_31_append_pydict(const char *nmea_payload, PyObject *dict,
                        const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_1_31 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "utc_day", msg.utc_day);
  DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
  DictSafeSetItem(dict, "utc_min", msg.utc_min);
  DictSafeSetItem(dict, "wind_ave", msg.wind_ave);
  DictSafeSetItem(dict, "wind_gust", msg.wind_gust);
  DictSafeSetItem(dict, "wind_dir", msg.wind_dir);
  DictSafeSetItem(dict, "wind_gust_dir", msg.wind_gust_dir);
  DictSafeSetItem(dict, "air_temp", msg.air_temp);
  DictSafeSetItem(dict, "rel_humid", msg.rel_humid);
  DictSafeSetItem(dict, "dew_point", msg.dew_point);
  DictSafeSetItem(dict, "air_pres", msg.air_pres);
  DictSafeSetItem(dict, "air_pres_trend", msg.air_pres_trend);
  DictSafeSetItem(dict, "horz_vis", msg.horz_vis);
  DictSafeSetItem(dict, "water_level", msg.water_level);
  DictSafeSetItem(dict, "water_level_trend", msg.water_level_trend);

  // TODO(schwehr): make this a list of dicts
  DictSafeSetItem(dict, "surf_cur_speed", msg.surf_cur_speed);
  DictSafeSetItem(dict, "surf_cur_dir", msg.surf_cur_dir);
  DictSafeSetItem(dict, "cur_speed_2", msg.cur_speed_2);
  DictSafeSetItem(dict, "cur_dir_2", msg.cur_dir_2);
  DictSafeSetItem(dict, "cur_depth_2", msg.cur_depth_2);
  DictSafeSetItem(dict, "cur_speed_3", msg.cur_speed_3);
  DictSafeSetItem(dict, "cur_dir_3", msg.cur_dir_3);
  DictSafeSetItem(dict, "cur_depth_3", msg.cur_depth_3);

  DictSafeSetItem(dict, "wave_height", msg.wave_height);
  DictSafeSetItem(dict, "wave_period", msg.wave_period);
  DictSafeSetItem(dict, "wave_dir", msg.wave_dir);
  DictSafeSetItem(dict, "swell_height", msg.swell_height);
  DictSafeSetItem(dict, "swell_period", msg.swell_period);
  DictSafeSetItem(dict, "swell_dir", msg.swell_dir);
  DictSafeSetItem(dict, "sea_state", msg.sea_state);
  DictSafeSetItem(dict, "water_temp", msg.water_temp);
  DictSafeSetItem(dict, "precip_type", msg.precip_type);
  DictSafeSetItem(dict, "salinity", msg.salinity);
  DictSafeSetItem(dict, "ice", msg.ice);

  return AIS_OK;
}


// no 32 broadcast

// DAC 200 - River Information System
AIS_STATUS
ais8_200_10_append_pydict(const char *nmea_payload, PyObject *dict,
                          const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_200_10 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "eu_id", msg.eu_id);
  DictSafeSetItem(dict, "length", msg.length);
  DictSafeSetItem(dict, "beam", msg.beam);
  DictSafeSetItem(dict, "ship_type", msg.ship_type);
  DictSafeSetItem(dict, "haz_cargo", msg.haz_cargo);
  DictSafeSetItem(dict, "draught", msg.draught);
  DictSafeSetItem(dict, "loaded", msg.loaded);
  DictSafeSetItem(dict, "speed_qual", msg.speed_qual);
  DictSafeSetItem(dict, "course_qual", msg.course_qual);
  DictSafeSetItem(dict, "heading_qual", msg.heading_qual);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}


// River Information System
AIS_STATUS
ais8_200_23_append_pydict(const char *nmea_payload, PyObject *dict,
                          const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_200_23 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "utc_year_start", msg.utc_year_start);
  DictSafeSetItem(dict, "utc_month_start", msg.utc_month_start);
  DictSafeSetItem(dict, "utc_day_start", msg.utc_day_start);
  DictSafeSetItem(dict, "utc_year_end", msg.utc_year_end);
  DictSafeSetItem(dict, "utc_month_end", msg.utc_month_end);
  DictSafeSetItem(dict, "utc_day_end", msg.utc_day_end);
  DictSafeSetItem(dict, "utc_hour_start", msg.utc_hour_start);
  DictSafeSetItem(dict, "utc_min_start", msg.utc_min_start);
  DictSafeSetItem(dict, "utc_hour_end", msg.utc_hour_end);
  DictSafeSetItem(dict, "utc_min_end", msg.utc_min_end);
  DictSafeSetItem(dict, "x1", "y1", msg.position1);
  DictSafeSetItem(dict, "x2", "y2", msg.position2);
  DictSafeSetItem(dict, "type", msg.type);
  DictSafeSetItem(dict, "min", msg.min);
  DictSafeSetItem(dict, "max", msg.max);
  DictSafeSetItem(dict, "classification", msg.classification);
  DictSafeSetItem(dict, "wind_dir", msg.wind_dir);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}


// River Information System
AIS_STATUS
ais8_200_24_append_pydict(const char *nmea_payload, PyObject *dict,
                          const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_200_24 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "country", msg.country);

  PyObject *id_list = PyList_New(4);
  for (size_t i = 0; i < 4; i++)
    PyList_SetItem(id_list, 0, PyLong_FromLong(msg.gauge_ids[i]));
  DictSafeSetItem(dict, "gauge_ids", id_list);

  PyObject *level_list = PyList_New(4);
  for (size_t i = 0; i < 4; i++)
    PyList_SetItem(level_list, 0, PyFloat_FromDouble(msg.levels[i]));
  DictSafeSetItem(dict, "levels", level_list);

  return AIS_OK;
}


// River Information System
AIS_STATUS
ais8_200_40_append_pydict(const char *nmea_payload, PyObject *dict,
                          const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_200_40 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "form", msg.form);
  DictSafeSetItem(dict, "dir", msg.dir);
  DictSafeSetItem(dict, "stream_dir", msg.stream_dir);
  DictSafeSetItem(dict, "status_raw", msg.status_raw);
  // TODO(schwehr): extract status components
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return AIS_OK;
}


// River Information System
AIS_STATUS
ais8_200_55_append_pydict(const char *nmea_payload, PyObject *dict,
                          const size_t pad) {
  assert(nmea_payload);
  assert(dict);
  assert(pad < 6);
  Ais8_200_55 msg(nmea_payload, pad);
  if (msg.had_error()) {
    return msg.get_error();
  }

  DictSafeSetItem(dict, "crew", msg.crew);
  DictSafeSetItem(dict, "passengers", msg.passengers);
  DictSafeSetItem(dict, "yet_more_personnel", msg.yet_more_personnel);

  PyObject *spare2_list = PyList_New(3);
  for (size_t i = 0; i < 3; i++)
    PyList_SetItem(spare2_list, 0,  PyLong_FromLong(msg.spare2[i]));
  DictSafeSetItem(dict, "spare2", spare2_list);

  return AIS_OK;
}

void
ais8_367_22_append_pydict(const char *nmea_payload, PyObject *dict,
                          const size_t pad) {
  Ais8_367_22 msg(nmea_payload, pad);  // TODO(schwehr): check for errors

  DictSafeSetItem(dict, "version", msg.version);
  DictSafeSetItem(dict, "link_id", msg.link_id);
  DictSafeSetItem(dict, "notice_type", msg.notice_type);
  // TODO(schwehr): are 8:1:22 and 8:367:22 tables the same?
  DictSafeSetItem(dict, "notice_type_str",
                  ais8_1_22_notice_names[msg.notice_type]);

  DictSafeSetItem(dict, "month", msg.month);  // This is UTC, not local time.
  DictSafeSetItem(dict, "day", msg.day);
  DictSafeSetItem(dict, "hour", msg.hour);
  DictSafeSetItem(dict, "minute", msg.minute);

  DictSafeSetItem(dict, "durations_minutes", msg.duration_minutes);

  PyObject *sub_area_list = PyList_New(msg.sub_areas.size());

  // Loop over sub_areas
  for (size_t i = 0; i < msg.sub_areas.size(); i++) {
    switch (msg.sub_areas[i]->getType()) {
    case AIS8_366_22_SHAPE_CIRCLE:  // or point
      {
        PyObject *sub_area = PyDict_New();
        Ais8_367_22_Circle *c =
            reinterpret_cast<Ais8_367_22_Circle*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_366_22_SHAPE_CIRCLE);
        if (c->radius_m == 0)
          DictSafeSetItem(sub_area, "sub_area_type_str", "point");
        else
          DictSafeSetItem(sub_area, "sub_area_type_str", "circle");

        DictSafeSetItem(sub_area, "x", "y", c->position);
        DictSafeSetItem(sub_area, "precision", c->precision);
        DictSafeSetItem(sub_area, "radius", c->radius_m);
        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_366_22_SHAPE_RECT:
      {
        PyObject *sub_area = PyDict_New();
        Ais8_367_22_Rect *c =
            reinterpret_cast<Ais8_367_22_Rect*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_366_22_SHAPE_RECT);
        DictSafeSetItem(sub_area, "sub_area_type_str", "rect");

        DictSafeSetItem(sub_area, "x", "y", c->position);
        DictSafeSetItem(sub_area, "precision", c->precision);
        DictSafeSetItem(sub_area, "e_dim_m", c->e_dim_m);
        DictSafeSetItem(sub_area, "n_dim_m", c->n_dim_m);
        DictSafeSetItem(sub_area, "orient_deg", c->orient_deg);

        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_366_22_SHAPE_SECTOR:
      {
        PyObject *sub_area = PyDict_New();
        Ais8_367_22_Sector *c =
            reinterpret_cast<Ais8_367_22_Sector*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", AIS8_366_22_SHAPE_SECTOR);
        DictSafeSetItem(sub_area, "sub_area_type_str", "sector");

        DictSafeSetItem(sub_area, "x", "y", c->position);
        DictSafeSetItem(sub_area, "precision", c->precision);
        DictSafeSetItem(sub_area, "radius", c->radius_m);
        DictSafeSetItem(sub_area, "left_bound_deg", c->left_bound_deg);
        DictSafeSetItem(sub_area, "right_bound_deg", c->right_bound_deg);

        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_366_22_SHAPE_POLYLINE:  // FALLTHROUGH
    case AIS8_366_22_SHAPE_POLYGON:
      {
        PyObject *sub_area = PyDict_New();
        Ais8_367_22_Poly *poly =
            reinterpret_cast<Ais8_367_22_Poly*>(msg.sub_areas[i]);

        DictSafeSetItem(sub_area, "sub_area_type", msg.sub_areas[i]->getType());
        if (msg.sub_areas[i]->getType() == AIS8_366_22_SHAPE_POLYLINE)
          DictSafeSetItem(sub_area, "sub_area_type_str", "polyline");
        else
          DictSafeSetItem(sub_area, "sub_area_type_str", "polygon");
        assert(poly->angles.size() == poly->dists_m.size());
        PyObject *angle_list = PyList_New(poly->angles.size());
        PyObject *dist_list = PyList_New(poly->angles.size());

        for (size_t pt_num = 0; pt_num < poly->angles.size(); pt_num++) {
          PyList_SetItem(angle_list, pt_num,
                         PyFloat_FromDouble(poly->angles[pt_num]));
          PyList_SetItem(dist_list, pt_num,
                         PyFloat_FromDouble(poly->dists_m[pt_num]));
        }

        DictSafeSetItem(sub_area, "angles", angle_list);
        DictSafeSetItem(sub_area, "dists_m", dist_list);

        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;
    case AIS8_366_22_SHAPE_TEXT:
      {
        PyObject *sub_area = PyDict_New();

        Ais8_367_22_Text *text =
            reinterpret_cast<Ais8_367_22_Text*>(msg.sub_areas[i]);
        DictSafeSetItem(sub_area, "sub_area_type", AIS8_366_22_SHAPE_TEXT);
        DictSafeSetItem(sub_area, "sub_area_type_str", "text");

        DictSafeSetItem(sub_area, "text", text->text);

        PyList_SetItem(sub_area_list, i, sub_area);
      }
      break;

    default:
      {}  // TODO(schwehr): Mark an unknown subarea or raise an exception.
    }
  }
  DictSafeSetItem(dict, "sub_areas", sub_area_list);
}

// AIS Binary broadcast messages.  There will be a huge number of subtypes
// If we don't know how to decode it, just return the dac, fi
PyObject*
ais8_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  assert(pad < 6);

  Ais8 msg(nmea_payload, pad);
  if (msg.had_error() && msg.get_error() != AIS_UNINITIALIZED) {
    PyErr_Format(ais_py_exception, "Ais8: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);
  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "dac", msg.dac);
  DictSafeSetItem(dict, "fi", msg.fi);

  AIS_STATUS status = AIS_UNINITIALIZED;

  switch (msg.dac) {
  case AIS_DAC_1_INTERNATIONAL:  // IMO.
    switch (msg.fi) {
      // See: ITU-R.M.1371-3 IFM messages Annex 5, Section 5 and IMO Circ 289
    case AIS_FI_8_1_0_TEXT:
      status = ais8_1_0_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_11_MET_HYDRO:  // Not to be used after 1 Jan 2013.
      status = ais8_1_11_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_13_FAIRWAY_CLOSED:  // Not to be used after 1 Jan 2013.
      // TODO(schwehr): untested - no messages found
      status = ais8_1_13_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_15_SHIP_AND_VOYAGE:  // Not after 1 Jan 2013.
      // TODO(schwehr): untested - no messages found
      status = ais8_1_15_append_pydict(nmea_payload, dict, pad);
      break;
      // 16 has conflicting definition in the old 1371-1: VTS targets.
    case AIS_FI_8_1_16_PERSONS_ON_BOARD:  // Not to be used after 1 Jan 2013.
      status = ais8_1_16_append_pydict(nmea_payload, dict, pad);
      break;
      // 17 has conflicting definitions in 1371-1: IFM 17: Ship waypoints
      // and/or route plan report.
    case AIS_FI_8_1_17_VTS_TARGET:  // Not to be used after Jan 2013.
      // TODO(schwehr): Untested. no messages found.
      status = ais8_1_17_append_pydict(nmea_payload, dict, pad);
      break;
      // ITU 1371-1 conflict: IFM 19: Extended ship static and voyage.
    case AIS_FI_8_1_19_TRAFFIC_SIGNAL:
      // TODO(schwehr): Untested. No messages found.
      status = ais8_1_19_append_pydict(nmea_payload, dict, pad);
      DictSafeSetItem(dict, "parsed", true);
      break;
      // 20:  Berthing data.
    case AIS_FI_8_1_21_WEATHER_OBS:
      // TODO(schwehr): untested - no messages found
      status = ais8_1_21_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_22_AREA_NOTICE:
      status = ais8_1_22_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_24_SHIP_AND_VOYAGE:
      status = ais8_1_24_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_26_SENSOR:
      status = ais8_1_26_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_27_ROUTE:
      status = ais8_1_27_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_29_TEXT:
      status = ais8_1_29_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_31_MET_HYDRO:
      status = ais8_1_31_append_pydict(nmea_payload, dict, pad);
      break;
      // ITU 1371-1 only: 3.10 - IFM 40: Number of persons on board.
    default:
      DictSafeSetItem(dict, "parsed", false);
      break;
    }
    break;
  case AIS_DAC_200_RIS:
    switch (msg.fi) {
    case AIS_FI_8_200_10_RIS_SHIP_AND_VOYAGE:
      status = ais8_200_10_append_pydict(nmea_payload, dict, pad);
      break;
      // 21: Addressed only
      // 22: Addressed only
    case AIS_FI_8_200_23_RIS_EMMA_WARNING:
      status = ais8_200_23_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_1_24_SHIP_AND_VOYAGE:
      status = ais8_200_24_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_200_40_RIS_ATON_SIGNAL_STATUS:
      status = ais8_200_40_append_pydict(nmea_payload, dict, pad);
      break;
    case AIS_FI_8_200_55_RIS_PERSONS_ON_BOARD:
      status = ais8_200_55_append_pydict(nmea_payload, dict, pad);
      break;
    default:
      DictSafeSetItem(dict, "parsed", false);
      break;
    }
    break;
    // TODO(schwehr): AIS_FI_8_366_22_AREA_NOTICE.
  case 367:  // United states.
    switch (msg.fi) {
    case 22:  // USCG Area Notice 2012 (v5?).
      ais8_367_22_append_pydict(nmea_payload, dict, pad);
      break;
    default:
      DictSafeSetItem(dict, "parsed", false);
      break;
    }
    break;
  default:
    DictSafeSetItem(dict, "parsed", false);
    // TODO(schwehr): raise exception or return standin?
  }

  if (status != AIS_OK) {
    PyErr_Format(ais_py_exception, "Ais8: %s",
                 AIS_STATUS_STRINGS[status]);
    return nullptr;
  }

  return dict;
}

// Aircraft position report
PyObject*
ais9_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais9 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais9: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "alt", msg.alt);
  DictSafeSetItem(dict, "sog", msg.sog);

  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "x", "y", msg.position);

  DictSafeSetItem(dict, "cog", msg.cog);
  DictSafeSetItem(dict, "timestamp", msg.timestamp);
  DictSafeSetItem(dict, "alt_sensor", msg.alt_sensor);
  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "dte", msg.dte);
  DictSafeSetItem(dict, "spare2", msg.spare2);
  DictSafeSetItem(dict, "assigned_mode", msg.assigned_mode);
  DictSafeSetItem(dict, "raim", msg.raim);

  DictSafeSetItem(dict, "sync_state", msg.sync_state);

  if (0 == msg.commstate_flag) {
    // SOTMDA
    DictSafeSetItem(dict, "slot_timeout", msg.slot_timeout);

    if (msg.received_stations_valid)
      DictSafeSetItem(dict, "received_stations", msg.received_stations);
    if (msg.slot_number_valid)
      DictSafeSetItem(dict, "slot_number", msg.slot_number);
    if (msg.utc_valid) {
      DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
      DictSafeSetItem(dict, "utc_min", msg.utc_min);
      DictSafeSetItem(dict, "utc_spare", msg.utc_spare);
    }
    if (msg.slot_offset_valid)
      DictSafeSetItem(dict, "slot_offset", msg.slot_offset);
  } else {
    // ITDMA
    DictSafeSetItem(dict, "slot_increment", msg.slot_increment);
    DictSafeSetItem(dict, "slots_to_allocate", msg.slots_to_allocate);
    DictSafeSetItem(dict, "keep_flag", msg.keep_flag);
  }

  return dict;
}

// 10 - ':' - UTC and date inquiry
PyObject*
ais10_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  assert(pad < 6);

  Ais10 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais10: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "dest_mmsi", msg.dest_mmsi);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return dict;
}

// msg 11 ';' - See msg 4_11

// 12 - '<' - Addressed safety related text
PyObject*
ais12_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais12 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais12: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "seq_num", msg.seq_num);
  DictSafeSetItem(dict, "dest_mmsi", msg.dest_mmsi);
  DictSafeSetItem(dict, "retransmitted", msg.retransmitted);

  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "text", msg.text);

  return dict;
}

// msg 13 - See msg 7

// 14 - '>' - Safety broadcast text
PyObject*
ais14_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais14 msg(nmea_payload, pad);

  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais14: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);
  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "text", msg.text);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return dict;
}

// 15 - '?' - Interrogation
PyObject*
ais15_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais15 msg(nmea_payload, pad);

  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais15: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "mmsi_1", msg.mmsi_1);
  DictSafeSetItem(dict, "msg_1_1", msg.msg_1_1);
  DictSafeSetItem(dict, "slot_offset_1_1", msg.slot_offset_1_1);

  DictSafeSetItem(dict, "spare2", msg.spare2);
  DictSafeSetItem(dict, "dest_msg_1_2", msg.dest_msg_1_2);
  DictSafeSetItem(dict, "slot_offset_1_2", msg.slot_offset_1_2);

  DictSafeSetItem(dict, "spare3", msg.spare3);
  DictSafeSetItem(dict, "mmsi_2", msg.mmsi_2);
  DictSafeSetItem(dict, "msg_2", msg.msg_2);
  DictSafeSetItem(dict, "slot_offset_2", msg.slot_offset_2);
  DictSafeSetItem(dict, "spare4", msg.spare4);

  return dict;
}

// 16 - '@' - Assigned mode command
PyObject*
ais16_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais16 msg(nmea_payload, pad);

  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais16: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "dest_mmsi_a", msg.dest_mmsi_a);
  DictSafeSetItem(dict, "offset_a", msg.offset_a);
  DictSafeSetItem(dict, "inc_a", msg.inc_a);

  if (-1 != msg.spare2) DictSafeSetItem(dict, "spare2", msg.spare2);
  if (-1 != msg.dest_mmsi_b) {
    DictSafeSetItem(dict, "dest_mmsi_b", msg.dest_mmsi_b);
    DictSafeSetItem(dict, "offset_b", msg.offset_b);
    DictSafeSetItem(dict, "inc_b", msg.inc_b);
  }

  return dict;
}

// 17 - 'A' - GNSS differential - TODO(schwehr): incomplete
PyObject*
ais17_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais17 msg(nmea_payload, pad);

  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais17: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return dict;
}

// 18 - 'B' - Class B position report.
PyObject*
ais18_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais18 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais18: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "sog", msg.sog);
  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "cog", msg.cog);
  DictSafeSetItem(dict, "true_heading", msg.true_heading);
  DictSafeSetItem(dict, "timestamp", msg.timestamp);

  DictSafeSetItem(dict, "spare2", msg.spare2);

  DictSafeSetItem(dict, "unit_flag", msg.unit_flag);
  DictSafeSetItem(dict, "display_flag", msg.display_flag);
  DictSafeSetItem(dict, "dsc_flag", msg.dsc_flag);
  DictSafeSetItem(dict, "band_flag", msg.band_flag);
  DictSafeSetItem(dict, "m22_flag", msg.m22_flag);
  DictSafeSetItem(dict, "mode_flag", msg.mode_flag);

  DictSafeSetItem(dict, "raim", msg.raim);

  DictSafeSetItem(dict, "commstate_flag", msg.commstate_flag);
  if (msg.slot_timeout_valid) {
    DictSafeSetItem(dict, "slot_timeout", msg.slot_timeout);
  }
  if (msg.slot_offset_valid) {
        DictSafeSetItem(dict, "slot_offset", msg.slot_offset);
  }
  if (msg.utc_valid) {
    DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
    DictSafeSetItem(dict, "utc_min", msg.utc_min);
    DictSafeSetItem(dict, "utc_spare", msg.utc_spare);
  }
  if (msg.slot_number_valid) {
    DictSafeSetItem(dict, "slot_number", msg.slot_number);
  }
  if (msg.received_stations_valid) {
    DictSafeSetItem(dict, "received_stations", msg.received_stations);
  }

  // ITDMA
  if (msg.slot_increment_valid) {
    DictSafeSetItem(dict, "slot_increment", msg.slot_increment);
    DictSafeSetItem(dict, "slots_to_allocate", msg.slots_to_allocate);
    DictSafeSetItem(dict, "keep_flag", msg.keep_flag);
  }

  if (msg.commstate_cs_fill_valid) {
    DictSafeSetItem(dict, "commstate_cs_fill", msg.commstate_cs_fill);
  }

  return dict;
}

// 19 - 'C' - Class B combined position report and ship data
PyObject*
ais19_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais19 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais19: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "spare", msg.spare);
  DictSafeSetItem(dict, "sog", msg.sog);
  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "cog", msg.cog);
  DictSafeSetItem(dict, "true_heading", msg.true_heading);
  DictSafeSetItem(dict, "timestamp", msg.timestamp);

  DictSafeSetItem(dict, "spare2", msg.spare2);

  DictSafeSetItem(dict, "name", msg.name);
  DictSafeSetItem(dict, "type_and_cargo", msg.type_and_cargo);
  DictSafeSetItem(dict, "dim_a", msg.dim_a);
  DictSafeSetItem(dict, "dim_b", msg.dim_b);
  DictSafeSetItem(dict, "dim_c", msg.dim_c);
  DictSafeSetItem(dict, "dim_d", msg.dim_d);
  DictSafeSetItem(dict, "fix_type", msg.fix_type);

  DictSafeSetItem(dict, "raim", msg.raim);

  DictSafeSetItem(dict, "dte", msg.dte);
  DictSafeSetItem(dict, "assigned_mode", msg.assigned_mode);
  DictSafeSetItem(dict, "spare3", msg.spare3);

  return dict;
}

// 20 - 'D' - data link management
PyObject*
ais20_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais20 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais20: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);
  DictSafeSetItem(dict, "spare", msg.spare);

  int list_size = 1;
  if (msg.group_valid_4) list_size = 4;
  else if (msg.group_valid_3) list_size = 3;
  else if (msg.group_valid_2) list_size = 2;

  PyObject *list = PyList_New(list_size);

  {
    PyObject *reservation = PyDict_New();
    DictSafeSetItem(reservation, "offset", msg.offset_1);
    DictSafeSetItem(reservation, "num_slots", msg.num_slots_1);
    DictSafeSetItem(reservation, "timeout", msg.timeout_1);
    DictSafeSetItem(reservation, "incr", msg.incr_1);
    PyList_SetItem(list, 0, reservation);
  }

  if (msg.group_valid_2) {
    PyObject *reservation = PyDict_New();
    DictSafeSetItem(reservation, "offset", msg.offset_2);
    DictSafeSetItem(reservation, "num_slots", msg.num_slots_2);
    DictSafeSetItem(reservation, "timeout", msg.timeout_2);
    DictSafeSetItem(reservation, "incr", msg.incr_2);
    PyList_SetItem(list, 1, reservation);
  }

  if (msg.group_valid_3) {
    PyObject *reservation = PyDict_New();
    DictSafeSetItem(reservation, "offset", msg.offset_3);
    DictSafeSetItem(reservation, "num_slots", msg.num_slots_3);
    DictSafeSetItem(reservation, "timeout", msg.timeout_3);
    DictSafeSetItem(reservation, "incr", msg.incr_3);
    PyList_SetItem(list, 2, reservation);
  }

  if (msg.group_valid_4) {
    PyObject *reservation = PyDict_New();
    DictSafeSetItem(reservation, "offset", msg.offset_4);
    DictSafeSetItem(reservation, "num_slots", msg.num_slots_4);
    DictSafeSetItem(reservation, "timeout", msg.timeout_4);
    DictSafeSetItem(reservation, "incr", msg.incr_4);
    PyList_SetItem(list, 3, reservation);
  }

  PyObject * reservations = PyUnicode_FromString("reservations");
  PyDict_SetItem(dict, reservations, list);
  Py_DECREF(reservations);
  Py_DECREF(list);

  return dict;
}


// 21 - 'E' - ATON Aid to Navigation
PyObject*
ais21_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais21 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais21: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);
  DictSafeSetItem(dict, "spare", msg.spare);

  DictSafeSetItem(dict, "aton_type", msg.aton_type);
  DictSafeSetItem(dict, "name", msg.name);
  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "dim_a", msg.dim_a);
  DictSafeSetItem(dict, "dim_b", msg.dim_b);
  DictSafeSetItem(dict, "dim_c", msg.dim_c);
  DictSafeSetItem(dict, "dim_d", msg.dim_d);
  DictSafeSetItem(dict, "fix_type", msg.fix_type);
  DictSafeSetItem(dict, "timestamp", msg.timestamp);
  DictSafeSetItem(dict, "off_pos", msg.off_pos);
  DictSafeSetItem(dict, "aton_status", msg.aton_status);
  DictSafeSetItem(dict, "raim", msg.raim);
  DictSafeSetItem(dict, "virtual_aton", msg.virtual_aton);
  DictSafeSetItem(dict, "assigned_mode", msg.assigned_mode);

  return dict;
}

// 22 - 'F' - Channel mangement
PyObject*
ais22_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais22 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais22: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);
  DictSafeSetItem(dict, "spare", msg.spare);

  DictSafeSetItem(dict, "chan_a", msg.chan_a);
  DictSafeSetItem(dict, "chan_b", msg.chan_b);
  DictSafeSetItem(dict, "txrx_mode", msg.txrx_mode);
  DictSafeSetItem(dict, "power_low", msg.power_low);

  if (msg.pos_valid) {
    DictSafeSetItem(dict, "x1", "y1", msg.position1);
    DictSafeSetItem(dict, "x2", "y2", msg.position2);
  } else {
    DictSafeSetItem(dict, "dest_mmsi_1", msg.dest_mmsi_1);
    DictSafeSetItem(dict, "dest_mmsi_2", msg.dest_mmsi_2);
  }

  DictSafeSetItem(dict, "chan_a_bandwidth", msg.chan_a_bandwidth);
  DictSafeSetItem(dict, "chan_b_bandwidth", msg.chan_b_bandwidth);
  DictSafeSetItem(dict, "zone_size", msg.zone_size);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  return dict;
}


// 23 - 'F' - Group assignment command
PyObject*
ais23_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais23 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais23: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);
  DictSafeSetItem(dict, "spare", msg.spare);

  DictSafeSetItem(dict, "x1", "y1", msg.position1);
  DictSafeSetItem(dict, "x2", "y2", msg.position2);

  DictSafeSetItem(dict, "station_type", msg.station_type);
  DictSafeSetItem(dict, "type_and_cargo", msg.type_and_cargo);
  DictSafeSetItem(dict, "spare2", msg.spare2);

  DictSafeSetItem(dict, "txrx_mode", msg.txrx_mode);
  DictSafeSetItem(dict, "interval_raw", msg.interval_raw);
  DictSafeSetItem(dict, "quiet", msg.quiet);
  DictSafeSetItem(dict, "spare3", msg.spare3);

  return dict;
}

// 24 - 'H' - Static data report
PyObject*
ais24_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais24 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais24: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "part_num", msg.part_num);

  switch (msg.part_num) {
  case 0:  // Part A
    DictSafeSetItem(dict, "name", msg.name);
    break;
  case 1:  // Part B
    DictSafeSetItem(dict, "type_and_cargo", msg.type_and_cargo);
    DictSafeSetItem(dict, "vendor_id", msg.vendor_id);
    DictSafeSetItem(dict, "callsign", msg.callsign);
    DictSafeSetItem(dict, "dim_a", msg.dim_a);
    DictSafeSetItem(dict, "dim_b", msg.dim_b);
    DictSafeSetItem(dict, "dim_c", msg.dim_c);
    DictSafeSetItem(dict, "dim_d", msg.dim_d);
    DictSafeSetItem(dict, "spare", msg.spare);
    break;
  case 2:  // FALLTHROUGH - not yet defined by ITU
  case 3:  // FALLTHROUGH - not yet defined by ITU
  default:
    // status = AIS_ERR_BAD_MSG_CONTENT;
    // TODO(schwehr): setup python exception
    return nullptr;
  }

  return dict;
}

// 25 - 'I' - Single slot binary message
PyObject*
ais25_to_pydict(const char *nmea_payload, const size_t pad) {
  assert(nmea_payload);
  Ais25 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais25: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  // TODO(schwehr) use_app_id
  if (msg.dest_mmsi_valid) DictSafeSetItem(dict, "dest_mmsi", msg.dest_mmsi);
  if (msg.use_app_id) {
    DictSafeSetItem(dict, "dac", msg.dac);
    DictSafeSetItem(dict, "fi", msg.fi);
  }

  // TODO(schwehr): handle payload

  return dict;
}

// 26 - 'J' - Multi-slot binary message with commstate
PyObject*
ais26_to_pydict(const char *nmea_payload, const size_t pad) {
  Ais26 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais26: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  if (msg.dest_mmsi_valid) DictSafeSetItem(dict, "dest_mmsi", msg.dest_mmsi);
  if (msg.use_app_id) {
    DictSafeSetItem(dict, "dac", msg.dac);
    DictSafeSetItem(dict, "fi", msg.fi);
  }

  // TODO(schwehr): handle payload

  DictSafeSetItem(dict, "sync_state", msg.sync_state);
  if (0 == msg.commstate_flag) {
    // SOTDMA
    if (msg.received_stations_valid)
      DictSafeSetItem(dict, "received_stations", msg.received_stations);
    if (msg.slot_number_valid)
      DictSafeSetItem(dict, "slot_number", msg.slot_number);
    if (msg.utc_valid) {
      DictSafeSetItem(dict, "utc_hour", msg.utc_hour);
      DictSafeSetItem(dict, "utc_min", msg.utc_min);
      DictSafeSetItem(dict, "utc_spare", msg.utc_spare);
    }
    if (msg.slot_offset_valid)
      DictSafeSetItem(dict, "slot_offset", msg.slot_offset);
  } else {
    // ITDMA
    DictSafeSetItem(dict, "slot_increment", msg.slot_increment);
    DictSafeSetItem(dict, "slots_to_allocate", msg.slots_to_allocate);
    DictSafeSetItem(dict, "keep_flag", msg.keep_flag);
  }
  return dict;
}


// 27 - 'K' - Short position report for satellite reception
PyObject*
ais27_to_pydict(const char *nmea_payload, const size_t pad) {
  Ais27 msg(nmea_payload, pad);
  if (msg.had_error()) {
    PyErr_Format(ais_py_exception, "Ais27: %s",
                 AIS_STATUS_STRINGS[msg.get_error()]);
    return nullptr;
  }

  PyObject *dict = ais_msg_to_pydict(&msg);

  DictSafeSetItem(dict, "position_accuracy", msg.position_accuracy);
  DictSafeSetItem(dict, "raim", msg.raim);
  DictSafeSetItem(dict, "nav_status", msg.nav_status);
  DictSafeSetItem(dict, "x", "y", msg.position);
  DictSafeSetItem(dict, "sog", msg.sog);
  DictSafeSetItem(dict, "cog", msg.cog);
  DictSafeSetItem(dict, "gnss", msg.gnss);
  DictSafeSetItem(dict, "spare", msg.spare);
  return dict;
}


extern "C" {

static PyObject *
decode(PyObject *self, PyObject *args) {
  int _pad;
  const char *nmea_payload;
  // TODO(schwehr): what to do about if no pad bits?  Maybe warn and set to 0?
  if (!PyArg_ParseTuple(args, "si", &nmea_payload, &_pad)) {
    _pad = 0;
    if (!PyArg_ParseTuple(args, "s", &nmea_payload)) {
      PyErr_Format(ais_py_exception, "ais.decode: expected (str, int)");
      return nullptr;
    }
  }
  const size_t pad = _pad;

  // The grand dispatcher
  switch (nmea_payload[0]) {
  case '1':  // FALLTHROUGH - Class A Position
  case '2':  // FALLTHROUGH
  case '3':
    return ais1_2_3_to_pydict(nmea_payload, pad);

  case '4':  // FALLTHROUGH - 4 - Basestation report
  case ';':  // 11 - UTC date response
    return ais4_11_to_pydict(nmea_payload, pad);

  case '5':  // 5 - Ship and Cargo
    return ais5_to_pydict(nmea_payload, pad);

  case '6':  // 6 - Addressed binary message
    return ais6_to_pydict(nmea_payload, pad);

  case '7':  // FALLTHROUGH - 7 - ACK for addressed binary message
  case '=':  // 13 - ASRM Ack  (safety message)
    return ais7_13_to_pydict(nmea_payload, pad);

  case '8':  // 8 - Binary broadcast message (BBM)
    return ais8_to_pydict(nmea_payload, pad);

  case '9':  // 9 - SAR Position
    return ais9_to_pydict(nmea_payload, pad);

  case ':':  // 10 - UTC Query
    return ais10_to_pydict(nmea_payload, pad);

    // ':' 11 - See 4

  case '<':  // 12 - ASRM
    return ais12_to_pydict(nmea_payload, pad);

    // 13 - See 7

  case '>':  // 14 - SRBM - Safety broadcast
    return ais14_to_pydict(nmea_payload, pad);

  case '?':  // 15 - Interrogation
    return ais15_to_pydict(nmea_payload, pad);

  case '@':  // 16 - Assigned mode command
    return ais16_to_pydict(nmea_payload, pad);

  case 'A':  // 17 - GNSS broadcast
    return ais17_to_pydict(nmea_payload, pad);

  case 'B':  // 18 - Position, Class B
    return ais18_to_pydict(nmea_payload, pad);

  case 'C':  // 19 - Position and ship, Class B
    return ais19_to_pydict(nmea_payload, pad);

  case 'D':  // 20 - Data link management
    return ais20_to_pydict(nmea_payload, pad);

  case 'E':  // 21 - Aids to navigation report
    return ais21_to_pydict(nmea_payload, pad);

  case 'F':  // 22 - Channel Management
    return ais22_to_pydict(nmea_payload, pad);

  case 'G':  // 23 - Group Assignment Command
    return ais23_to_pydict(nmea_payload, pad);

  case 'H':  // 24 - Static data report
    return ais24_to_pydict(nmea_payload, pad);

  case 'I':  // 25 - Single slot binary message - addressed or broadcast
    // TODO(schwehr): handle payloads
    return ais25_to_pydict(nmea_payload, pad);

  case 'J':  // 26 - Multi slot binary message with comm state
    return ais26_to_pydict(nmea_payload, pad);  // TODO(schwehr): payloads

  case 'K':  // 27 - Long-range AIS broadcast message
    return ais27_to_pydict(nmea_payload, pad);

  case 'L':  // 28 - UNKNOWN
    PyErr_Format(ais_py_exception, "ais.decode: message 28 (L) not handled");
    break;

  default:
    PyErr_Format(ais_py_exception, "ais.decode: unknown message - %c",
                 nmea_payload[0]);
  }

  return nullptr;
}

static PyMethodDef ais_methods[] = {
  {"decode", decode, METH_VARARGS, "Return a dictionary for a NMEA string"},
  {nullptr, nullptr, 0, nullptr},  // Sentinel
};

// Python module initialization

#if PY_MAJOR_VERSION >= 3

static struct PyModuleDef aismodule = {
  PyModuleDef_HEAD_INIT,
  "_ais",  // Module name.
  nullptr,  // Module documentation.
  -1,  // Says the module keeps state in global variables.
  ais_methods
};

PyMODINIT_FUNC PyInit__ais(void) {
  PyObject *module = PyModule_Create(&aismodule);

  if (module == nullptr)
    return nullptr;

  ais_py_exception = PyErr_NewException(exception_name, nullptr, nullptr);
  Py_INCREF(ais_py_exception);
  PyModule_AddObject(module, exception_short, ais_py_exception);
  return module;
}

#else  // Python 2.7

void init_ais(void) {
  PyObject *module = Py_InitModule("_ais", ais_methods);

  if (module == nullptr) {
    return;
  }

  ais_py_exception = PyErr_NewException(
      const_cast<char *>(exception_name), nullptr, nullptr);
  Py_INCREF(ais_py_exception);
  PyModule_AddObject(module, exception_short, ais_py_exception);
}

#endif  // PY_MAJOR_VERSION

}  // extern "C"

}  // namespace libais
