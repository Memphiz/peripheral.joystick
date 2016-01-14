/*
 *      Copyright (C) 2015 Garrett Brown
 *      Copyright (C) 2015 Team XBMC
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "ButtonMap.h"

#include "platform/util/timeutils.h"

using namespace JOYSTICK;

#define RESOURCE_LIFETIME_MS  2000 // 2 seconds

CButtonMap::CButtonMap(const std::string& strResourcePath) :
  m_strResourcePath(strResourcePath),
  m_timestamp(-1)
{
}

CButtonMap::CButtonMap(const std::string& strResourcePath, const CDevice& device) :
  m_strResourcePath(strResourcePath),
  m_device(device),
  m_timestamp(-1)
{
}

bool CButtonMap::GetFeatures(const std::string& controllerId, FeatureVector& features)
{
  Refresh();

  ButtonMap::const_iterator it = m_buttonMap.find(controllerId);
  if (it != m_buttonMap.end())
  {
    features = it->second;
    return true;
  }

  return false;
}

bool CButtonMap::MapFeatures(const std::string& controllerId, const FeatureVector& features)
{
  FeatureVector& oldFeatures = m_buttonMap[controllerId];

  // TODO: Optimize case when features are unchanged
  bool bChanged = true; // TODO

  if (bChanged)
  {
    oldFeatures = features;

    if (Save())
    {
      m_timestamp = PLATFORM::GetTimeMs();
      return true;
    }
  }

  return false;
}

bool CButtonMap::ResetButtonMap(const std::string& controllerId)
{
  FeatureVector& features = m_buttonMap[controllerId];

  if (!features.empty())
  {
    features.clear();

    if (Save())
    {
      m_timestamp = PLATFORM::GetTimeMs();
      return true;
    }
  }

  return false;
}

bool CButtonMap::Refresh(void)
{
  const int64_t expires = m_timestamp + RESOURCE_LIFETIME_MS;
  const int64_t now = PLATFORM::GetTimeMs();

  if (now >= expires)
  {
    if (!Load())
      return false;

    m_timestamp = now;
  }

  return true;
}
