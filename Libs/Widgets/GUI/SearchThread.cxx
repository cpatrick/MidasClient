/******************************************************************************
 * Copyright 2011 Kitware Inc.
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
 *****************************************************************************/
#include "SearchThread.h"

#include "mwsSearch.h"
#include "mdoObject.h"

SearchThread::SearchThread()
{
}

SearchThread::~SearchThread()
{
}

void SearchThread::SetWords(std::vector<std::string> words)
{
  this->m_Words = words;
}

void SearchThread::SetResults(std::vector<mdo::Object *>* results)
{
  this->m_Results = results;
}

void SearchThread::run()
{
  *m_Results = mws::Search::SearchServer(m_Words);
}

