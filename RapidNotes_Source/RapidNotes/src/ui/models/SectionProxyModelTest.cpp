#include "src/ui/models/SectionProxyModel.h"
#include "src/core/ModelContract.h"
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QElapsedTimer>
#include <QDebug>
#include <cassert>

using namespace QuarkMeta;

class TestFilterProxyModel : public QSortFilterProxyModel {
public:
    bool filterFolders = true;
    bool filterFiles = true;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override {
        Q_UNUSED(sourceParent);
        QModelIndex idx = sourceModel()->index(sourceRow, 0);
        QString typeStr = idx.data(TypeRole).toString();
        if (typeStr == "folder") return filterFolders;
        return filterFiles;
    }
};

void runSectionProxyModelTests() {
    qDebug() << "=== Starting Extended SectionProxyModel Invariant Self-Tests ===";

    QStandardItemModel sourceModel;
    for (int i = 0; i < 10; ++i) {
        auto* folderItem = new QStandardItem(QString("Folder %1").arg(i));
        folderItem->setData("folder", TypeRole);
        sourceModel.appendRow(folderItem);
    }
    for (int i = 0; i < 20; ++i) {
        auto* fileItem = new QStandardItem(QString("File %1").arg(i));
        fileItem->setData("file", TypeRole);
        sourceModel.appendRow(fileItem);
    }

    TestFilterProxyModel filterModel;
    filterModel.setSourceModel(&sourceModel);

    SectionProxyModel proxyModel;
    proxyModel.setSourceModel(&filterModel);

    auto verifyInvariants = [&](const QString& context) {
        int fc = proxyModel.folderCount();
        int fic = proxyModel.fileCount();
        bool hasFolderHeader = (!proxyModel.rowCount() == 0 && proxyModel.index(0, 0).data(SectionHeaderRole).toBool() && proxyModel.index(0, 0).data(SectionKindRole).toInt() == 1);

        assert(hasFolderHeader == (fc > 0));

        if (fc > 0 && fic > 0) {
            int fileHeaderRow = proxyModel.isFolderCollapsed() ? 1 : 1 + fc;
            assert(proxyModel.index(fileHeaderRow, 0).data(SectionHeaderRole).toBool());
            assert(proxyModel.index(fileHeaderRow, 0).data(SectionKindRole).toInt() == 2);
        }

        // Verify mapToSource DisplayRole & TypeRole correctness
        for (int r = 0; r < proxyModel.rowCount(); ++r) {
            QModelIndex pIdx = proxyModel.index(r, 0);
            if (pIdx.data(SectionHeaderRole).toBool()) continue;
            QModelIndex sIdx = proxyModel.mapToSource(pIdx);
            assert(sIdx.isValid());
            assert(sIdx.data(Qt::DisplayRole).toString() == pIdx.data(Qt::DisplayRole).toString());
        }
        qDebug() << "[PASS] Invariant verified for:" << context;
    };

    verifyInvariants("Initial state");

    // Test: 10 Filter Toggle Cycles (Folders)
    for (int cycle = 0; cycle < 10; ++cycle) {
        filterModel.filterFolders = false;
        filterModel.invalidate();
        verifyInvariants(QString("Cycle %1: Hide Folders").arg(cycle));

        filterModel.filterFolders = true;
        filterModel.invalidate();
        verifyInvariants(QString("Cycle %1: Show Folders").arg(cycle));
    }

    // Test: 10 Filter Toggle Cycles (Files)
    for (int cycle = 0; cycle < 10; ++cycle) {
        filterModel.filterFiles = false;
        filterModel.invalidate();
        verifyInvariants(QString("Cycle %1: Hide Files").arg(cycle));

        filterModel.filterFiles = true;
        filterModel.invalidate();
        verifyInvariants(QString("Cycle %1: Show Files").arg(cycle));
    }

    // Test: Collapsed State Cycles
    proxyModel.setFolderCollapsed(true);
    for (int cycle = 0; cycle < 5; ++cycle) {
        filterModel.filterFolders = false; filterModel.invalidate();
        verifyInvariants(QString("Collapsed Cycle %1: Hide Folders").arg(cycle));

        filterModel.filterFolders = true; filterModel.invalidate();
        verifyInvariants(QString("Collapsed Cycle %1: Show Folders").arg(cycle));
    }
    proxyModel.setFolderCollapsed(false);

    qDebug() << "=== All SectionProxyModel Invariant Self-Tests Passed Successfully ===";
}

#ifdef TEST_STANDALONE
int main(int argc, char** argv) {
    runSectionProxyModelTests();
    return 0;
}
#endif
