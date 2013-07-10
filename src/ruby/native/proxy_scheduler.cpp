/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ruby.h"

#include <iostream>

#include "proxy_scheduler.hpp"
#include "module.hpp"
#include "mesos_scheduler_driver_impl.hpp"

using namespace mesos;

using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::map;

namespace mesos {
namespace ruby {

void ProxyScheduler::registered(SchedulerDriver* driver,
                                const FrameworkID& frameworkId,
                                const MasterInfo& masterInfo)
{
  VALUE fid = NULL;
  VALUE minfo = NULL;
  VALUE res = NULL;

  fid = createPythonProtobuf(frameworkId, "FrameworkID");
  if (fid == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  minfo = createPythonProtobuf(masterInfo, "MasterInfo");
  if (minfo == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("registered"), 3, impl, fid, minfo);
  if (res == NULL) {
    cerr << "Failed to call scheduler's registered" << endl;
    driver->abort();
  }

}


void ProxyScheduler::reregistered(SchedulerDriver* driver,
                                  const MasterInfo& masterInfo)
{
  VALUE minfo = NULL;
  VALUE res = NULL;

  minfo = createPythonProtobuf(masterInfo, "MasterInfo");
  if (minfo == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("reregistered"), 2, impl, minfo);
  if (res == NULL) {
    cerr << "Failed to call scheduler's reregistered" << endl;
    driver->abort();
  }

}


void ProxyScheduler::disconnected(SchedulerDriver* driver)
{
  VALUE res = NULL;

  res = rb_funcall(impl->pythonScheduler, rb_intern("disconnected"), 1, impl);
  if (res == NULL) {
    cerr << "Failed to call scheduler's disconnected" << endl;
    driver->abort();
  }

}


void ProxyScheduler::resourceOffers(SchedulerDriver* driver,
                                    const vector<Offer>& offers)
{
  VALUE list = NULL;
  VALUE res = NULL;

  list = PyList_New(offers.size());
  if (list == NULL) {
    driver->abort();
  }
  for (size_t i = 0; i < offers.size(); i++) {
    VALUE offer = createPythonProtobuf(offers[i], "Offer");
    if (offer == NULL) {
      driver->abort();
    }
    PyList_SetItem(list, i, offer); // Steals the reference to offer
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("resource_offers"), 2, impl, list);

  if (res == NULL) {
    cerr << "Failed to call scheduler's resourceOffer" << endl;
    driver->abort();
  }

}


void ProxyScheduler::offerRescinded(SchedulerDriver* driver,
                                    const OfferID& offerId)
{
  VALUE oid = NULL;
  VALUE res = NULL;

  oid = createPythonProtobuf(offerId, "OfferID");
  if (oid == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("offer_rescinded"), 2, impl, oid);
  if (res == NULL) {
    cerr << "Failed to call scheduler's offerRescinded" << endl;
    driver->abort();
  }

}


void ProxyScheduler::statusUpdate(SchedulerDriver* driver,
                                  const TaskStatus& status)
{
  VALUE stat = NULL;
  VALUE res = NULL;

  stat = createPythonProtobuf(status, "TaskStatus");
  if (stat == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("status_update"), 2, impl, stat);
  if (res == NULL) {
    cerr << "Failed to call scheduler's statusUpdate" << endl;
    driver->abort();
  }

}


void ProxyScheduler::frameworkMessage(SchedulerDriver* driver,
                                      const ExecutorID& executorId,
                                      const SlaveID& slaveId,
                                      const string& data)
{
  VALUE eid = NULL;
  VALUE sid = NULL;
  VALUE res = NULL;

  eid = createPythonProtobuf(executorId, "ExecutorID");
  if (eid == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  sid = createPythonProtobuf(slaveId, "SlaveID");
  if (sid == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("framework_message"), 4, impl, eid, sid, rb_str_new(data.data(), data.length()));
  if (res == NULL) {
    cerr << "Failed to call scheduler's frameworkMessage" << endl;
    driver->abort();
  }

}


void ProxyScheduler::slaveLost(SchedulerDriver* driver, const SlaveID& slaveId)
{
  VALUE sid = NULL;
  VALUE res = NULL;

  sid = createPythonProtobuf(slaveId, "SlaveID");
  if (sid == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("slave_lost"), 2, impl, sid);
  if (res == NULL) {
    cerr << "Failed to call scheduler's slaveLost" << endl;
    driver->abort();
  }

}


void ProxyScheduler::executorLost(SchedulerDriver* driver,
                                  const ExecutorID& executorId,
                                  const SlaveID& slaveId,
                                  int status)
{
  VALUE executorIdObj = NULL;
  VALUE slaveIdObj = NULL;
  VALUE res = NULL;

  executorIdObj = createPythonProtobuf(executorId, "ExecutorID");
  slaveIdObj = createPythonProtobuf(slaveId, "SlaveID");

  if (executorIdObj == NULL || slaveIdObj == NULL) {
    driver->abort(); // createPythonProtobuf will have set an exception
  }

  res = rb_funcall(impl->pythonScheduler, rb_intern("executor_lost"), 4, impl, executorIdObj, slaveIdObj, INT2FIX(status));
  if (res == NULL) {
    cerr << "Failed to call scheduler's executorLost" << endl;
    driver->abort();
  }

}


void ProxyScheduler::error(SchedulerDriver* driver, const string& message)
{
  VALUE res = rb_funcall(impl->pythonScheduler, rb_intern("error"), 2, impl, rb_str_new(message.data(), message.length()));
  if (res == NULL) {
    cerr << "Failed to call scheduler's error" << endl;
  }
}

} // namespace python {
} // namespace mesos {
