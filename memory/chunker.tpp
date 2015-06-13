CHUNKER_TEMPLATE
Chunker<CHUNKER_ARGS>::Chunker()
{
    std::cerr << "trying to create chunker<" << max_chunks << ", " << max_chunk_size << ", " << corrected_chunk_size << ">" << std::endl;

    const uint64_t BUFFER_SIZE = max_chunks * sizeof(Block);

    m_raw_data.reset(new (std::nothrow) uint8_t[BUFFER_SIZE]);
    if (!m_raw_data)
        throw std::runtime_error(std::string("unable to allocate ").append(std::to_string(BUFFER_SIZE)).append(" bytes"));

    m_blocks = reinterpret_cast<Block *>(m_raw_data.get());
    m_next = m_blocks;

    Block * current = m_next;
    for (uint64_t i = 1; i < max_chunks; ++i)
    {
        current->next = i;
        ++current;
    }
    current->next = max_chunks;
}

CHUNKER_TEMPLATE
Chunker<CHUNKER_ARGS>::~Chunker()
{

}

CHUNKER_TEMPLATE
std::shared_ptr<uint8_t> Chunker<CHUNKER_ARGS>::get_shared(uint64_t size)
{
    std::cerr << "trying to create chunk of size " << size << std::endl;
    assert(size);
    if (size > corrected_chunk_size)
        throw std::runtime_error("size is too big");

    uint8_t * out = create();
    if (out)
        return std::shared_ptr<uint8_t>(out, [=](uint8_t * ptr){destroy(ptr);});

    throw std::runtime_error("unable to allocate chunk");
}

CHUNKER_TEMPLATE
uint8_t * Chunker<CHUNKER_ARGS>::create()
{
    if (!m_next)
        return nullptr;

    uint8_t * output = m_next->data;
    if (m_next->next == max_chunks)
        m_next = nullptr;
    else
        m_next = reinterpret_cast<Block *>(&m_blocks[m_next->next]);

    return output;
}

CHUNKER_TEMPLATE
void Chunker<CHUNKER_ARGS>::destroy(uint8_t *ptr)
{
    std::cerr << "destoying chunk" << std::endl;
    Block * block = reinterpret_cast<Block *>(ptr);
    if (!m_next)
        block->next = max_chunks;
    else
        block->next = m_next - m_blocks;

    m_next = block;
}

