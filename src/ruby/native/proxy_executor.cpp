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

#include "proxy_executor.hpp"
#include "module.hpp"
#include "mesos_executor_driver_impl.hpp"

using namespace mesos;

using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::map;

namespace mesos {
namespace ruby {

void ProxyExecutor::registered(ExecutorDriver* driver,
                               const ExecutorInfo& executorInfo,
                               const FrameworkInfo& frameworkInfo,
                               const SlaveInfo& slaveInfo)
{
  VALUE executorInfoObj = NULL;
  VALUE frameworkInfoObj = NULL;
  VALUE slaveInfoObj = NULL;
  VALUE res = NULL;

  //TODO(burke)
  executorInfoObj = createPythonProtobuf(executorInfo, "ExecutorInfo");
  frameworkInfoObj = createPythonProtobuf(frameworkInfo, "FrameworkInfo");
  slaveInfoObj = createPythonProtobuf(slaveInfo, "SlaveInfo");

  if (executorInfoObj == NULL ||
      frameworkInfoObj == NULL ||
      slaveInfoObj == NULL) {
    driver->abort();
  }

  res = rb_funcall(impl->rubyExecutor, rb_intern("registered"), 4,
      impl,
      executorInfoObj,
      frameworkInfoObj,
      slaveInfoObj);

  if (res == NULL) {
    cerr << "Failed to call executor registered" << endl;
    driver->abort();
  }
}


void ProxyExecutor::reregistered(ExecutorDriver* driver,
                                 const SlaveInfo& slaveInfo)
{
  VALUE slaveInfoObj = NULL;
  VALUE res = NULL;

  // TODO(burke)
  slaveInfoObj = createPythonProtobuf(slaveInfo, "SlaveInfo");

  if (slaveInfoObj == NULL) {
    driver->abort();
  }

  res = rb_funcall(impl->rubyExecutor, rb_intern("reregistered"), 2, impl, slaveInfoObj);

  if (res == NULL) {
    cerr << "Failed to call executor re-registered" << endl;
    driver->abort();
  }

}


void ProxyExecutor::disconnected(ExecutorDriver* driver)
{
  VALUE res = rb_funcall(impl->pythonExecutor, rb_intern("disconnected"), 1, ipml);

  if (res == NULL) {
    cerr << "Failed to call executor's disconnected" << endl;
    driver->abort();
  }
}


void ProxyExecutor::launchTask(ExecutorDriver* driver,
                               const TaskInfo& task)
{
  VALUE taskObj = NULL;
  VALUE res = NULL;

  // TODO(burke)
  taskObj = createPythonProtobuf(task, "TaskInfo");
  if (taskObj == NULL) {
    driver->abort()
  }

  res = rb_funcall(impl->rubyExecutor, rb_intern("launch_task"), 2, impl, taskObj);

  if (res == NULL) {
    cerr << "Failed to call executor's launchTask" << endl;
    driver->abort();
  }
}


void ProxyExecutor::killTask(ExecutorDriver* driver,
                             const TaskID& taskId)
{
  VALUE taskIdObj = NULL;
  VALUE res = NULL;

  // TODO(burke)
  taskIdObj = createPythonProtobuf(taskId, "TaskID");

  if (taskIdObj == NULL) {
    driver->abort();
  }

  res = rb_funcall(impl->rubyExecutor, rb_intern("kill_task"), 2, impl, taskIdObj);

  if (res == NULL) {
    cerr << "Failed to call executor's killTask" << endl;
    driver->abort();
  }

}


void ProxyExecutor::frameworkMessage(ExecutorDriver* driver,
                                     const string& data)
{
  VALUE res = NULL;

  res = rb_funcall(impl->rubyExecutor, rb_intern("framework_message"), impl, rb_str_new(data.data(), data.length()))

  if (res == NULL) {
    cerr << "Failed to call executor's frameworkMessage" << endl;
    driver->abort();
  }

}


void ProxyExecutor::shutdown(ExecutorDriver* driver)
{
  VALUE res = rb_funcall(impl->rubyExecutor, rb_intern("shutdown"), 1, impl);

  if (res == NULL) {
    cerr << "Failed to call executor's shutdown" << endl;
    driver->abort();
  }
}


void ProxyExecutor::error(ExecutorDriver* driver, const string& message)
{
  VALUE res = rb_funcall(impl->rubyExecutor, rb_intern("error"), 2, impl, rb_str_new(message.data(), message.length()));
  if (res == NULL) {
    cerr << "Failed to call executor's error" << endl;
  }
}

} // namespace ruby {
} // namespace mesos {
