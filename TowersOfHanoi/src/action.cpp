#include <worldobject.h>
#include <action.h>

Action::Action(WorldObject* associatedObject)
:
	m_associatedObject(associatedObject),
	m_completed(false)
{
}

bool Action::isComplete() const
{
	return m_completed;
}
