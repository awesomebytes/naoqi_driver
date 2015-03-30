/*
 * Copyright 2015 Aldebaran
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef RECORDER_HPP
#define RECORDER_HPP

#include <string>

#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
# include <boost/thread/mutex.hpp>

#include <ros/ros.h>
#include <rosbag/bag.h>
#include <rosbag/view.h>
#include <geometry_msgs/TransformStamped.h>

#include "alrosbridge/recorder/globalrecorder.hpp"

namespace alros
{
namespace recorder
{

/**
* @brief Recorder concept interface
* @note this defines an private concept struct,
* which each instance has to implement
* @note a type erasure pattern in implemented here to avoid strict inheritance,
* thus each possible recorder instance has to implement the virtual functions mentioned in the concept
*/
class Recorder
{

public:

  /**
  * @brief Constructor for recorder interface
  */
  template<typename T>
  Recorder( T rec ):
    recPtr_( boost::make_shared<RecorderModel<T> >(rec) )
  {};

  /**
  * @brief triggers the writing process of the concrete recorder instance
  */
  void write( )
  {
    recPtr_->write();
  }

  /**
  * @brief checks if the recorder is correctly initialized on the ros-master
  @ @return bool value indicating true for success
  */
  bool isInitialized() const
  {
    return recPtr_->isInitialized();
  }

  /**
  * @brief checks if the recorder has a subscription and is hence allowed to record
  * @return bool value indicating true for number of sub > 0
  */
  bool isSubscribed() const
  {
    return recPtr_->isSubscribed();
  }

  /**
  * @brief initializes/resets the recorder into ROS with a given nodehandle,
  * this will be called at first for initialization or again when master uri has changed
  * @param ros NodeHandle to advertise the recorder on
  */
  //void reset( alros::recorder::GlobalRecorder& gr )
  void reset( boost::shared_ptr<alros::recorder::GlobalRecorder> gr)
  {
    recPtr_->reset( gr );
  }

private:

  /**
  * BASE concept struct
  */
  struct RecorderConcept
  {
    virtual ~RecorderConcept(){};
    virtual void write() = 0;
    virtual bool isInitialized() const = 0;
    virtual void subscribe(bool state) const = 0;
    virtual bool isSubscribed() const = 0;
    virtual std::string topic() const = 0;
    virtual void reset( boost::shared_ptr<alros::recorder::GlobalRecorder> gr ) = 0;
  };


  /**
  * templated instances of base concept
  */
  template<typename T>
  struct RecorderModel : public RecorderConcept
  {
    RecorderModel( const T& other ):
      recorder_( other )
    {}

    void reset( boost::shared_ptr<alros::recorder::GlobalRecorder> gr )
    {
      recorder_.reset( gr );
    }

    void write()
    {
      recorder_.write();
    }

    bool isInitialized()
    {
      return recorder_.isInitialized();
    }

    void subscribe(bool state)
    {
      recorder_.subscribe( state );
    }

    bool isSubscribed()
    {
      return recorder_.isSubscribed();
    }

    std::string topic() const
    {
      return recorder_.topic();
    }

    T recorder_;
  };

  boost::shared_ptr<RecorderConcept> recPtr_;

}; // class recorder

} // recorder
} //alros

#endif
