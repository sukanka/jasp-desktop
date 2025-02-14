//
// Copyright (C) 2013-2018 University of Amsterdam
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public
// License along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//

#include "terms.h"

#include <sstream>

#include <QDataStream>
#include <QIODevice>
#include <QSet>
#include "utilities/qutils.h"
#include "variableinfo.h"

using namespace std;

Terms::Terms(const QList<QList<QString> > &terms, Terms *parent)
{
	_parent = parent;
	set(terms);
}

Terms::Terms(const QList<QString> &terms, Terms *parent)
{
	_parent = parent;
	set(terms);
}

Terms::Terms(const std::vector<std::vector<string> > &terms, Terms *parent)
{
	_parent = parent;
	set(terms);
}

Terms::Terms(const std::vector<string> &terms, Terms *parent)
{
	_parent = parent;
	set(terms);
}

Terms::Terms(const QList<Term> &terms, Terms *parent)
{
	_parent = parent;
	set(terms);
}

Terms::Terms(Terms *parent)
{
	_parent = parent;
}

void Terms::set(const std::vector<Term> &terms, bool isUnique)
{
	_terms.clear();

	for(const Term &term : terms)
		add(term, isUnique);
}

void Terms::set(const std::vector<string> &terms, bool isUnique)
{
	_terms.clear();

	for(const Term &term : terms)
		add(term, isUnique);
}

void Terms::set(const std::vector<std::vector<string> > &terms, bool isUnique)
{
	_terms.clear();

	for(const Term &term : terms)
		add(term, isUnique);
}

void Terms::set(const QList<Term> &terms, bool isUnique)
{
	_terms.clear();

	for(const Term &term : terms)
		add(term, isUnique);
}

void Terms::set(const Terms &terms, bool isUnique)
{
	_terms.clear();
	_hasDuplicate = terms.hasDuplicate();

	for(const Term &term : terms)
		add(term, isUnique);
}

void Terms::set(const QList<QList<QString> > &terms, bool isUnique)
{
	_terms.clear();

	for(const QList<QString> &term : terms)
		add(Term(term), isUnique);
}

void Terms::set(const QList<QString> &terms, bool isUnique)
{
	_terms.clear();

	for(const QString &term : terms)
		add(Term(term), isUnique);
}

void Terms::setSortParent(const Terms &parent)
{
	_parent = &parent;
}

void Terms::removeParent() {
	_parent = nullptr;
}

void Terms::add(const Term &term, bool isUnique)
{
	if (!isUnique || _hasDuplicate)
	{
		if (!_hasDuplicate && contains(term)) _hasDuplicate = true;
		_terms.push_back(term);
	}
	else if (_parent != nullptr)
	{
		vector<Term>::iterator itr = _terms.begin();
		int result = -1;

		for (; itr != _terms.end(); itr++)
		{
			result = termCompare(term, *itr);
			if (result >= 0)
				break;
		}

		if (result > 0)
			_terms.insert(itr, term);
		else if (result == 0)
		{
			itr->setDraggable(term.isDraggable());
			itr->setTypes(term.types());
		}
		else if (result < 0)
			_terms.push_back(term);
	}
	else
	{
		int i = indexOf(term);
		if (i < 0)
			_terms.push_back(term);
		else
		{
			_terms.at(i).setDraggable(term.isDraggable());
			_terms.at(i).setTypes(term.types());
		}
	}
}

void Terms::insert(int index, const Term &term)
{
	if (_parent == nullptr)
	{
		vector<Term>::iterator itr = _terms.begin();

		for (int i = 0; i < index; i++)
			itr++;

		_terms.insert(itr, term);
	}
	else
	{
		add(term);
	}
}

void Terms::insert(int index, const Terms &terms)
{
	if (_parent == nullptr)
	{
		vector<Term>::iterator itr = _terms.begin();

		for (int i = 0; i < index; i++)
			itr++;

		_terms.insert(itr, terms.begin(), terms.end());
	}
	else
	{
		add(terms);
	}
}

void Terms::add(const Terms &terms)
{
	_hasDuplicate = _hasDuplicate || terms.hasDuplicate();

	for(const Term & term : terms)
		add(term);
}

const Term& Terms::at(size_t index) const
{
	return _terms.at(index);
}

Term &Terms::at(size_t index)
{
	return _terms.at(index);
}

bool Terms::contains(const Term &term) const
{
	return std::find(_terms.begin(), _terms.end(), term) != _terms.end();
}

bool Terms::contains(const std::string & component)
{
	return contains(tq(component));
}

int Terms::indexOf(const QString &component) const
{
	int i = 0;
	for(const Term &term : _terms)
	{
		if (term.contains(component))
			return i;
		i++;
	}

	return -1;
}

int Terms::indexOf(const Term &term) const
{
	auto it = std::find(_terms.begin(), _terms.end(), term);
	if (it == _terms.end())
		return -1;
	else
		return it - _terms.begin();
}


bool Terms::contains(const QString & component)
{
	for(const Term &term : _terms)
	{
		if (term.contains(component))
			return true;
	}

	return false;
}

vector<string> Terms::asVector() const
{
	vector<string> items;

	for(const Term &term : _terms)
		items.push_back(term.asString());

	return items;
}

std::set<std::string> Terms::asSet() const
{
	std::set<std::string> items;

	for(const Term &term : _terms)
		for(std::string termComp : term.scomponents())
			items.insert(termComp);

	return items;
}

vector<vector<string> > Terms::asVectorOfVectors() const
{
	vector<vector<string> > items;

	for(const Term &term : _terms)
	{
		vector<string> components = term.scomponents();
		items.push_back(components);
	}

	return items;
}

QList<QString> Terms::asQList() const
{
	QList<QString> items;

	for(const Term &term : _terms)
		items.append(term.asQString());

	return items;
}

QList<QList<QString> > Terms::asQListOfQLists() const
{
	QList<QList<QString> > items;

	for(const Term &term : _terms)
	{
		QList<QString> components = term.components();
		items.append(components);
	}

	return items;
}

Terms Terms::sortComponents(const Terms &terms) const
{
	QList<Term> ts;

	for (const Term &term : terms)
		ts.append(sortComponents(term));

	return Terms(ts);
}

Terms Terms::crossCombinations() const
{
	if (_terms.size() <= 1)
		return *this;

	Terms t;

	for (uint r = 1; r <= _terms.size(); r++)
	{
		vector<bool> v(_terms.size());
		fill(v.begin() + r, v.end(), true);

		do {

			vector<string> combination;
			columnTypeVec types;

			for (uint i = 0; i < _terms.size(); i++) {
				if (!v[i])
				{
					vector<string> components = _terms.at(i).scomponents();
					columnTypeVec termTypes = _terms.at(i).types();
					combination.insert(combination.end(), components.begin(), components.end());
					types.insert(types.end(), termTypes.begin(), termTypes.end());
				}
			}

			Term newTerm(combination);
			newTerm.setTypes(types);
			t.add(newTerm);

		} while (std::next_permutation(v.begin(), v.end()));
	}

	return t;
}

Terms Terms::wayCombinations(int ways) const
{
	Terms t;

	for (int r = ways; r <= ways; r++)
	{
		vector<bool> v(_terms.size());
		std::fill(v.begin() + r, v.end(), true);

		do {

			vector<string> combination;
			columnTypeVec types;

			for (uint i = 0; i < _terms.size(); ++i) {
				if (!v[i])
				{
					vector<string> components = _terms.at(i).scomponents();
					columnTypeVec termTypes = _terms.at(i).types();
					combination.insert(combination.end(), components.begin(), components.end());
					types.insert(types.end(), termTypes.begin(), termTypes.end());
				}
			}

			Term newTerm(combination);
			newTerm.setTypes(types);
			t.add(newTerm);

		} while (std::next_permutation(v.begin(), v.end()));
	}

	return t;
}

Terms Terms::ffCombinations(const Terms &terms)
{
	// full factorial combinations

	Terms combos = terms.crossCombinations();

	Terms newTerms;

	newTerms.add(*this);
	newTerms.add(combos);

	for (uint i = 0; i < _terms.size(); i++)
	{
		for (uint j = 0; j < combos.size(); j++)
		{
			QStringList termComponents = _terms.at(i).components();
			termComponents.append(combos.at(j).components());
			columnTypeVec types = _terms.at(i).types();
			columnTypeVec coTypes = combos.at(j).types();
			types.insert(types.end(), coTypes.begin(), coTypes.end());

			Term newTerm(termComponents);
			newTerm.setTypes(types);
			newTerms.add(newTerm);
		}
	}

	newTerms.add(terms);

	return newTerms;
}

Terms Terms::combineTerms(JASPControl::CombinationType type)
{
	Terms combinedTerms;

	size_t nbTerms = size();

	switch (type)
	{
	case JASPControl::CombinationType::CombinationCross:
		combinedTerms = crossCombinations();
		break;
	case JASPControl::CombinationType::CombinationInteraction:
		combinedTerms = wayCombinations(nbTerms);
		break;
	case JASPControl::CombinationType::Combination2Way:
		combinedTerms = nbTerms < 2 ? Terms() : wayCombinations(2);
		break;
	case JASPControl::CombinationType::Combination3Way:
		combinedTerms = nbTerms < 3 ? Terms() : wayCombinations(3);
		break;
	case JASPControl::CombinationType::Combination4Way:
		combinedTerms = nbTerms < 4 ? Terms() : wayCombinations(4);
		break;
	case JASPControl::CombinationType::Combination5Way:
		combinedTerms = nbTerms < 5 ? Terms() : wayCombinations(5);
		break;
	case JASPControl::CombinationType::NoCombination:
	default:
		combinedTerms = *this;
		break;
	}

	return combinedTerms;
}


string Terms::asString() const
{
	if (_terms.size() == 0)
		return "";

	stringstream ss;

	ss << _terms.at(0).asString();

	for (size_t i = 1; i < _terms.size(); i++)
		ss << ", " << _terms.at(i).asString();

	return ss.str();
}

bool Terms::operator==(const Terms &terms) const
{
	return _terms == terms._terms;
}

bool Terms::operator!=(const Terms &terms) const
{
	return _terms != terms._terms;
}

bool Terms::strictlyEquals(const Terms &terms) const
{
	bool isEqual = _terms == terms._terms;

	for (size_t i = 0; isEqual && (i < _terms.size()); i++)
		isEqual = _terms.at(i).isDraggable() == terms._terms.at(i).isDraggable();

	return isEqual;
}

void Terms::setDraggable(bool draggable)
{
	for (Term& term : _terms)
		term.setDraggable(draggable);
}

void Terms::setUndraggableTerms(const Terms& undraggableTerms)
{
	std::vector<Term> newTerms = undraggableTerms._terms;
	for (Term& term : newTerms)
		term.setDraggable(false);

	// Then add only the draggabled terms that are not in undraggableTerms and that are draggable
	// All undraggable terms that are not in undraggableTerms will be then automatically removed.
	for (Term term : _terms)
	{
		if (term.isDraggable() && !undraggableTerms.contains(term))
			newTerms.push_back(term);
	}

	_terms = newTerms;
}

Json::Value Terms::types(bool onlyChanged, const VariableInfoConsumer* info) const
{
	Json::Value types(Json::arrayValue);

	auto changedType = [&, onlyChanged, info] (const QString& term, columnType type) -> Json::Value
	{
		if (onlyChanged && info && (columnType)info->requestInfo(VariableInfo::VariableType, term).toInt() == type)
			return Json::nullValue;
		else
			return columnTypeToString(type);
	};

	for (const Term& term : _terms)
	{
		if (term.components().size() == 1)
			types.append(changedType(term.asQString(), term.type()));
		else
		{
			Json::Value componentTypes(Json::arrayValue);
			columnTypeVec termTypes = term.types();
			int i = 0;
			for (const QString component : term.components())
			{
				componentTypes.append(changedType(component, termTypes.size() > i ? termTypes[i] : columnType::unknown));
				i++;
			}
			types.append(componentTypes);
		}
	}

	return types;
}

void Terms::set(const QByteArray & array, bool isUnique)
{
	QDataStream stream(array);

	if (stream.atEnd())
		throw exception();

	int count;
	stream >> count;

	clear();

	while ( ! stream.atEnd())
	{
		QStringList variable;
		stream >> variable;
		add(Term(variable), isUnique);
	}
}

Term Terms::sortComponents(const Term &term) const
{
	QStringList components = term.components();
	std::sort(components.begin(), components.end(), [&](const QString & l, const QString & r)->bool{ return componentLessThan(l,r); });
	return Term(components);
}

int Terms::rankOf(const QString &component) const
{
	if (_parent == nullptr)
		return 0;

	int index = 0;

	for(const Term& compare : _parent->terms())
	{
		if (compare.asQString() == component)
			break;
		index++;
	}

	return index;
}

int Terms::termCompare(const Term &t1, const Term &t2) const
{
	if (_parent == nullptr)
		return 1;

	if (t1.size() < t2.size())
		return 1;
	if (t1.size() > t2.size())
		return -1;

	for (uint i = 0; i < t1.size(); i++)
	{
		int t1Rank = rankOf(t1.at(i));
		int t2Rank = rankOf(t2.at(i));

		if (t1Rank < t2Rank)
			return 1;
		if (t1Rank > t2Rank)
			return -1;
	}

	return 0;
}

bool Terms::termLessThan(const Term &t1, const Term &t2) const
{
	return termCompare(t1, t2) > 0;
}

bool Terms::componentLessThan(const QString &c1, const QString &c2) const
{
	return rankOf(c1) < rankOf(c2);
}

void Terms::remove(const Terms &terms)
{
	for(const Term &term : terms)
	{
		vector<Term>::iterator itr = find(_terms.begin(), _terms.end(), term);
		if (itr != _terms.end())
			_terms.erase(itr);
	}
}

void Terms::remove(size_t pos, size_t n)
{
	vector<Term>::iterator itr = _terms.begin();

	for (size_t i = 0; i < pos && itr != _terms.end(); i++)
		itr++;

	for (; n > 0 && itr != _terms.end(); n--)
		_terms.erase(itr);
}

void Terms::replace(int pos, const Term &term)
{
	size_t pos_t = size_t(pos);
	if (pos_t <_terms.size())
	{
		remove(pos_t);
		insert(pos, term);
	}
}

bool Terms::discardWhatDoesntContainTheseComponents(const Terms &terms)
{
	bool changed = false;

	_terms.erase(
		std::remove_if(
			_terms.begin(),
			_terms.end(),
			[&](Term& existingTerm)
			{
				for (const string &str : existingTerm.scomponents())
					if (! terms.contains(str))
					{
						changed = true;
						return true;
					}

				return false;
			}
		),
		_terms.end()
	);

	return changed;
}

bool Terms::discardWhatDoesContainTheseComponents(const Terms &terms)
{
	bool changed = false;

	_terms.erase(
		std::remove_if(
			_terms.begin(),
			_terms.end(),
			[&](Term& existingTerm)
			{
				for (const Term &term : terms)
					for (const QString &component : term.components())
						if (existingTerm.contains(component))
						{
							changed			= true;
							return true;
						}


				return false;
			}),
		_terms.end()
	);

	return changed;
}

bool Terms::discardWhatDoesContainTheseTerms(const Terms &terms)
{
	bool changed = false;

	_terms.erase(
		std::remove_if(
			_terms.begin(),
			_terms.end(),
			[&](const Term& existingTerm)
			{
				for (const Term &term : terms)
					if (existingTerm.containsAll(term))
					{
						changed = true;
						return true;
					}

				return false;
			}),
		_terms.end()
	);

	return changed;
}

bool Terms::discardWhatIsntTheseTerms(const Terms &terms, Terms *discarded)
{
	bool changed = false;

	_terms.erase(
		std::remove_if(
			_terms.begin(),
			_terms.end(),
			[&](Term& term)
			{
				if (!term.asString().empty() && !terms.contains(term))
				{
					if (discarded != nullptr)
						discarded->add(term);

					changed = true;
					return true;
				}

				return false;
			}),
		_terms.end()
	);

	return changed;
}

void Terms::clear()
{
	_terms.clear();
}

size_t Terms::size() const
{
	return _terms.size();
}

const std::vector<Term> &Terms::terms() const
{
	return _terms;
}

Terms::const_iterator Terms::begin() const
{
	return _terms.begin();
}

Terms::const_iterator Terms::end() const
{
	return _terms.end();
}

Terms::iterator Terms::begin()
{
	return _terms.begin();
}

Terms::iterator Terms::end()
{
	return _terms.end();
}

void Terms::remove(const Term &term)
{
	vector<Term>::iterator itr = std::find(_terms.begin(), _terms.end(), term);
	if (itr != end())
		_terms.erase(itr);
}

QSet<int> Terms::replaceVariableName(const std::string & oldName, const std::string & newName)
{
	QSet<int> change;

	int i = 0;
	for(Term & t : _terms)
	{
		if (t.replaceVariableName(oldName, newName))
			change.insert(i);
		i++;
	}

	return change;
}
