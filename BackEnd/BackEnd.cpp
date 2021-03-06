#include "BackEnd.hpp"
#include "AST/Program.hpp"
#include "AST/ThreeAddressInstruction.hpp"
#include "AST/StringTable.hpp"
#include "Allocation.hpp"
#include <fstream>

std::set<std::shared_ptr<cs6300::BasicBlock>> seenBlocks;

void cs6300::emitMIPS(std::shared_ptr<cs6300::BasicBlock> block,
                      std::ostream& fout)
{

  if (!block || (seenBlocks.find(block) != seenBlocks.end())) return;
  seenBlocks.insert(block);
  fout << block->getLabel() << ":\t" << std::endl;
  for (auto&& i : block->instructions)
  {
    fout << i;
  }
  if (block->branchTo)
  {
    fout << "\tbne $" << block->branchOn << ", $zero, "
         << block->branchTo->getLabel() << std::endl;
  }
  if (block->jumpTo)
  {
    fout << "\tj " << block->jumpTo->getLabel() << std::endl;
  }

  emitMIPS(block->branchTo, fout);
  emitMIPS(block->jumpTo, fout);
}
void cs6300::emitMIPS(cs6300::FlowGraph cfg, std::ostream& fout)
{
  seenBlocks.clear();
  emitMIPS(cfg.first, fout);
}

void writeStringTable(std::ostream& fout)
{
  auto string_table = cs6300::StringTable::instance()->getTable();
  for (auto s : string_table)
  {
    fout << "SL" << s.second << ": \t.asciiz " << s.first << std::endl;
  }
}

void cs6300::writeMIPS(
  std::shared_ptr<IntermediateRepresentationProgram> program,
  std::string filename)
{
  locRegAlloc(program->main);
  std::ofstream fout(filename);
  fout << ".text" << std::endl << ".globl main" << std::endl << "main:"
       << "\tla $gp, GA" << std::endl << "\tori $fp, $sp, 0" << std::endl;

  emitMIPS(program->main, fout);

  for (auto&& f : program->functions)
  {
    fout << "F" << f.first.getLabel() << ":" << std::endl;
    locRegAlloc(f.second);
    emitMIPS(f.second, fout);
  }

  fout << ".data" << std::endl;
  writeStringTable(fout);

  fout << ".align 2" << std::endl;
  fout << "GA:" << std::endl;
}
