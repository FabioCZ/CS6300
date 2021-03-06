#include "OrExpression.hpp"
cs6300::OrExpression::OrExpression(std::shared_ptr<Expression> lhs,
                                   std::shared_ptr<Expression> rhs)
  : m_lhs(lhs), m_rhs(rhs)
{
}

std::shared_ptr<cs6300::BasicBlock> cs6300::OrExpression::emit() const
{
  return emitBinaryOp(ThreeAddressInstruction::Or, getLabel(), m_lhs, m_rhs);
}

std::shared_ptr<cs6300::Type> cs6300::OrExpression::type() const
{
  return BuiltInType::getBool();
}

int cs6300::OrExpression::value() const
{
  if (!isConst()) return 0;
  return m_lhs->value() || m_rhs->value();
}
bool cs6300::OrExpression::isConst() const
{
  if (!m_lhs) return false;
  if (!m_rhs) return false;
  return m_lhs->isConst() && m_rhs->isConst();
}

std::string cs6300::OrExpression::name() const
{
  return "\"||\"";
}

std::vector<std::string> cs6300::OrExpression::ASTDot() const
{
  std::vector<std::string> lines;
  join(m_lhs, lines, id());
  return join(m_rhs, lines, id());
}
