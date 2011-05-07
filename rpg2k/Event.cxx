#include "Event.hxx"
#include "Stream.hxx"


namespace rpg2k
{
	namespace structure
	{
		Instruction::Instruction()
		: code_(-1)
		{
		}
		Instruction::Instruction(std::istream& s)
		{
			code_ = stream::readBER(s);
			nest_ = stream::readBER(s);

			Binary b;
			stream::readWithSize(s, b);
			stringArgument_.assign(reinterpret_cast<char const*>(b.data()), b.size());

			int const argNum = stream::readBER(s);

			argument_.resize(argNum, VAR_DEF_VAL);
			for(int i = 0; i < argNum; i++) argument_[i] = stream::readBER(s);
		}

		int32_t Instruction::at(unsigned index) const
		{ rpg2k_assert(index < argument_.size()); return argument_[index]; }
		int32_t Instruction::operator [](unsigned index) const
		{ rpg2k_assert(index < argument_.size()); return argument_[index]; }
		int32_t& Instruction::at(unsigned index)
		{ rpg2k_assert(index < argument_.size()); return argument_[index]; }
		int32_t& Instruction::operator [](unsigned index)
		{ rpg2k_assert(index < argument_.size()); return argument_[index]; }

		size_t Instruction::serializedSize() const
		{
			unsigned ret =
				stream::berSize(code_) + stream::berSize(nest_) +
				stream::berSize(stringArgument_.size()) + stringArgument_.size() +
				stream::berSize(argument_.size());
			for(unsigned i = 0; i < argument_.size(); i++) ret += stream::berSize(argument_[i]);
			return ret;
		}
		std::ostream& Instruction::serialize(std::ostream& s) const
		{
			stream::writeBER(s, code_);
			stream::writeBER(s, nest_);
			stream::writeWithSize(s, stringArgument_);
			stream::writeBER(s, argument_.size());
			for(unsigned i = 0; i < argument_.size(); i++) stream::writeBER(s, argument_[i]);
			return s;
		}

		Event::Event(Binary const& b)
		{
			io::stream<io::array_source> s(io::array_source(reinterpret_cast<char const*>(b.data()), b.size()));
			init(s);
		}

		void Event::init(std::istream& s)
		{
			while(!stream::isEOF(s)) {
				data_.push_back(Instruction(s));
				if(data_.back().code() == 12110) { // check for label
					if(!label_.insert(std::make_pair(data_.back()[0], data_.size() - 1)).second) {
						rpg2k_assert(false);
					}
				}
			}
		}

		size_t Event::serializedSize() const
		{
			return this->serializedSize(0);
		}
		size_t Event::serializedSize(unsigned const offset) const
		{
			unsigned ret = 0;
			for(Data::const_iterator i = data_.begin() + offset; i < data_.end(); ++i) {
				ret += i->serializedSize();
			}
			return ret;
		}
		std::ostream& Event::serialize(std::ostream& s) const
		{
			for(Data::const_iterator i = data_.begin(); i < data_.end(); ++i) {
				i->serialize(s);
			}
			return s;
		}

		void Event::resize(unsigned size)
		{
			data_.resize(size);
		}
	} // namespace structure
} // namespace rpg2k
