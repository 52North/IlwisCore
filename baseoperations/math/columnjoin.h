#ifndef COLUMNJOIN_H
#define COLUMNJOIN_H
namespace Ilwis {
namespace BaseOperations{
class ColumnJoin : public NumericOperation
{
    public:
        ColumnJoin();
        ColumnJoin(quint64 metaid, const Ilwis::OperationExpression &expr);
        bool execute(ExecutionContext *ctx, SymbolTable& symTable);
        static OperationImplementation * create(quint64 metaid,const Ilwis::OperationExpression& expr);

        void addInsertChangedDataToTempTableStmt(const QString &tmpTable, const ITable table);
        QString columnNamesCommaSeparated(const ITable table) const;
        QString createInsertValueString(QVariant value, const ColumnDefinition &coldef) const;
        QString trimAndRemoveLastCharacter(const QString &string) const;
        static quint64 createMetadata();
        State prepare(ExecutionContext *ctx,const SymbolTable&);

        void createTable(const QString &tmpTable, const ITable table);

        void deleteTable(const QString &tmpTable);
private:
        ITable _baseTable;
        ITable _foreignTable;
        ITable _outputTable;
        ICoverage _inputCoverage;
        ICoverage _outputCoverage;
        std::vector<bool> _retainedBaseTableColumns;
        std::vector<bool> _retainedForeignTableColumns;

        QString _primaryKeyColumn = sUNDEF;
        QString _foreignKeyColumn = sUNDEF;
        NEW_OPERATION(ColumnJoin);
      };
 }
}
#endif // COLUMNJOIN_H
